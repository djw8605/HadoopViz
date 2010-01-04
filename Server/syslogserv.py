#!/usr/bin/python

import os
import re
import sys
import time
import Queue
import datetime
import optparse
import threading
from socket import *
from select import *

if '/opt/vdt/gratia/probe/common' not in sys.path:
    sys.path.append('/opt/vdt/gratia/probe/common')

import Gratia

syslogport = 514

# Sample client trace:
# 2009-07-24 07:59:13,012 INFO org.apache.hadoop.hdfs.server.datanode.DataNode.clienttrace: src: /172.16.1.144:50010, dest: /172.16.1.104:36825, bytes: 66048, op: HDFS_READ, cliID: DFSClient_-541008184, srvID: DS-824630517-172.16.1.144-50010-1234487017305, blockid: blk_-2994819911287448111_787295

class GratiaRecordUploader(object):

    def __init__(self, probeConfig):
        Gratia.Initialize(probeConfig)
        self._queue = Queue.Queue()
        self.stop = False

    def getHostByAddr(self, addr):
        try:
            addr = addr.split(":")[0]
            return gethostbyaddr(addr)[0]
        except:
            print "Could not resolve: %s" % addr
            return addr

    def queue(self, record):
        self._queue.put(record)

    def shutdown(self, block=True):
        self.stop = True
        self.thread.join()

    def createAndSend(self, record):
        record['src'] = self.getHostByAddr(record['src'])
        record['dest'] = self.getHostByAddr(record['dest'])
        r = Gratia.UsageRecord('Storage')
        r.AdditionalInfo('Protocol', record['protocol'])
        r.AdditionalInfo('Destination', record['dest'])
        r.AdditionalInfo('Source', record['src'])
        r.AdditionalInfo('Operation', record['op'])
        if record['op'] == 'HDFS_WRITE':
            r.AdditionalInfo('IsNew', 1)
        else:
            r.AdditionalInfo('IsNew', 0)
        startTime = record['startTime'].strftime("%Y-%m-%dT%H:%M:%SZ")
        r.StartTime(startTime)
        r.Grid(record['grid'])
        connectionTimeStr = 'PT%iS' % record['duration']
        r.Network(record['bytes'], 'b', connectionTimeStr, 'total', 'transfer')
        r.WallDuration(connectionTimeStr)
        r.Status('0')
        print "Sending %s, %s, %s, %s, %i" % (startTime, record['src'],
            record['dest'], record['op'], record['bytes'])
        try:
            print Gratia.Send(r)
        except Exception, e:
            print e
            print "Unable to send record!"

    def process(self):
        while self.stop == False or (not self._queue.empty()):
            try:
                self.createAndSend(self._queue.get(block=True, timeout=.5))
            except Queue.Empty, e:
                pass

    def run(self):
        t = threading.Thread(target=self.process)
        t.setDaemon(True)
        t.setName("Gratia Processing Thread")
        self.thread = t
        t.start()

ONE_MINUTE = datetime.timedelta(0, 60)
ONE_HOUR = datetime.timedelta(0, 3600)

class GratiaRecordKeeper(object):

    def __init__(self, probeConfig, per_minute=False):
        self.uploader = GratiaRecordUploader(probeConfig)
        self.uploader.run()
        self.default = {'protocol': 'hadoop', 'grid': 'OSG', 'duration': 0}
        if per_minute:
            self.default['duration'] = 60
        else:
            self.default['duration'] = 3600
        self.default_len = len(self.default)
        self.delay_for_old_records = 30 # in seconds
        self.delay_for_old_records = datetime.timedelta(0,
            self.delay_for_old_records)
        self.sweeper_interval = 600 # in seconds
        self.last_sweep = time.time()
        self.per_minute = per_minute
        self.records = {}
        self.year = str(datetime.datetime.now().year) + ' '

    def spill(self):
        """
        Send in all records not already given to Gratia, regardless of whether
        they may be complete.
        """
        for record in self.records.values():
            self.uploader.queue(record)
        self.record = {}

    def shutdown(self, block=True):
        self.spill()
        self.uploader.shutdown(block=block)

    def sweep(self):
        now = datetime.datetime.now()
        self.year = str(now.year) + ' '
        if self.per_minute:
            time_bin = datetime.datetime(now.year, now.month, now.day,
                now.hour, now.minute, 0)
            interval = ONE_MINUTE
        else:
            time_bin = datetime.datetime(now.year, now.month, now.day, now.hour,
                0, 0)
            interval = ONE_HOUR
        delay = interval + self.delay_for_old_records
        delete_list = []
        for key in self.records:
            if key[0] < time_bin and key[0] + delay < now:
                self.uploader.queue(self.records[key])
                delete_list.append(key)
        print "Sending %i finished records." % len(delete_list)
        for key in delete_list:
            del self.records[key]
        print "Currently, %i records in memory." % len(self.records)
        self.last_sweep = time.time()

    def __call__(self, *args, **kw):
        return self.record(*args, **kw)

    def record(self, date, src, dest, bytes, op):
        time_tuple = time.strptime(self.year + date, '%Y %b %d %H:%M:%S')
        if self.per_minute:
            time_bin = datetime.datetime(*time_tuple[:5])
        else:
            time_bin = datetime.datetime(*time_tuple[:4])

        # As necessary, clean out old records.
        if self.last_sweep + self.sweeper_interval < time.time():
            self.sweep()

        my_tuple = (time_bin, src, dest, op)
        my_record = self.records.get(my_tuple, None)
        if my_record:
            my_record['bytes'] += bytes
        else:
            my_record = dict(self.default)
            my_record['src'] = src
            my_record['dest'] = dest
            my_record['op'] = op
            my_record['bytes'] = bytes
            my_record['startTime'] = time_bin
            self.records[my_tuple] = my_record
            #print "Total in-progress records: %i" % len(self.records)

class SysLogServ(object):
    
    def __init__(self, probeConfig, port=5679):
        if probeConfig:
            self.recordKeeper = GratiaRecordKeeper(probeConfig)
        else:
            self.recordKeeper = None
        self.port = port
        self.udpservSocket = socket(AF_INET, SOCK_DGRAM)
        self.syslogSocket =  socket(AF_INET, SOCK_DGRAM)
        self.tcpservSocket = socket(AF_INET,SOCK_STREAM)
        self.bufsize = 2048
        self.hostnames = {}
        
        self.InitRegex()
       
    def InitRegex(self):
        self.srcregex = re.compile('src:')
        self.destregex = re.compile('dest:')
        self.clientregex = re.compile('.*<\d+>([\w]+ \d{2,2} \d{2,2}:\d{2,2}:\d{2,2}).*INFO.*DataNode\.clienttrace: src: /([\d\./]+):\d+, dest: /([\d\./]+):\d+, bytes: (\d+), op: (\w+)')
        self.receiveblock = re.compile('Receiving block')
        self.gridftp = re.compile("GRIDFTP")
        
    def GetSrcDest(self, line):
        srcpos = self.srcregex.search(line).end()+2
        src = line[srcpos: line.index(':', srcpos)]
        destpos = self.destregex.search(line).end()+2
        dest = line[destpos: line.index(':', destpos)]
        return src,dest
        
    def Start(self):
        
        # bind to the port, listen from everywhere
        self.udpservSocket.bind(('', self.port))
        self.syslogSocket.bind(('', syslogport))
        self.tcpservSocket.bind(('', self.port))
        self.tcpservSocket.listen(3)
        
        second = time.time()
        counter = 0
        
        self.connlist = []
        selectList = []
        selectList.append(self.tcpservSocket)
        selectList.append(self.udpservSocket)
        selectList.append(self.syslogSocket)
        #event loop
        while 1:
            parse = None
            results = select( selectList, [], [])
            
            #packets per second statistics
            if int(time.time()) != second:
                print "pps: %i" % counter
                counter = 0
                second = int(time.time())
            else:
                counter = counter + 1
            
            #check for closing connections
            for con in self.connlist:
                if results[0].count(con) is not 0:
                    print "closing connection"
                    con.close()
                    connlist.remove(con)
                    
            #check for incoming connections
            if results[0].count(self.tcpservSocket) is not 0:
                print "incoming connection"
                consoc, conaddr = self.tcpservSocket.accept()
                self.connlist.append(consoc)
                
            #incoming packets from syslog
            
            if results[0].count(self.udpservSocket) is not 0:
                data, addr = self.udpservSocket.recvfrom(self.bufsize)
                self.Parse(data)
            if results[0].count(self.syslogSocket) is not 0:
                data, addr = self.syslogSocket.recvfrom(self.bufsize)
                self.Parse(data)



    def Parse(self, data):
        client_match = self.clientregex.match(data)
        if self.receiveblock.search(data):
            src,dest = self.GetSrcDest(data)
            self.SendToConnected(self.connlist, "recvblock", src, dest)
        elif client_match:
            time, src, dest, bytes, op = client_match.groups()
            self.SendToConnected(self.connlist, "clienttrace", src, dest)
            if self.recordKeeper:
                self.recordKeeper(time, src, dest, int(bytes), op)
        elif self.gridftp.search(data):
            #print "Received gridftp packet"
            gridftparr = self.ParseGridftp(data)
            if gridftparr is None:
                return
            if gridftparr[2] == "READ":
                operation = "float"
                #print "Sending Drop"
            elif gridftparr[2] == "WRITE":
                operation = "drop"
                #print "Sending float"
            else:
                print "Unknown GRIDFTP operation: %s" % gridftparr[2]
                
            if self.hostnames.has_key(gridftparr[0]):
                src = self.hostnames[gridftparr[0]]
            else:
                src = gethostbyname(gridftparr[0])
                self.hostnames[gridftparr[0]] = src
                print "Adding %s as %s" % (gridftparr[0], src)
            self.SendToConnected(self.connlist, operation, src, gridftparr[1], gridftparr[3])
            #print data
            #self.SendToConnected(self.connlist, "drop", src, dest, extra)
        
                    

#<150>GRIDFTP: red-gridftp1 64.253.183.243:33299 READ 1048576


    def ParseGridftp(self, packet):
        """
        Returns an array:
        0: src
        1: dest
        2: operation
        3: operation size
        """
        try:
            words = packet.split()
            words = words[1:]
            return words
        except:
            print "Error reading a gridftp packet: %s" % packet
            return None



    def SendToConnected(self, connlist, type="", src="", dest="", extra=None):
        for con in connlist:
            try:
                if extra is None:
                    con.send("%s %s %s\n" % (type, src, dest))
                else:
                    con.send("%s %s %s %s\n" % (type, src, dest, extra))
            except:
                connlist.remove(con)
                try:
                    con.close()
                except:
                    pass


    def Close(self):
        for con in self.connlist:
            con.close()
        self.recordKeeper.shutdown()



def parse():
    parser = optparse.OptionParser()
    parser.add_option("-p", "--probeConfig", dest="probeConfig",
        help="Location of the Gratia ProbeConfig file.")
    options, args = parser.parse_args()

    if options.probeConfig and not os.path.exists(options.probeConfig):
        print "The specified location of the Gratia ProbeConfig, %s, does not" \
            " exist." % options.probeConfig
        raise Exception()
    return options, args

def main():
    try:
        options, args = parse()
    except:
        raise
        sys.exit(1)

    sysserv = SysLogServ(options.probeConfig)

    try:
        sysserv.Start()
    except KeyboardInterrupt:
        sysserv.Close()


if __name__ == "__main__":
    main()


