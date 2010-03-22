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

syslogport = 514

# Sample client trace:
# 2009-07-24 07:59:13,012 INFO org.apache.hadoop.hdfs.server.datanode.DataNode.clienttrace: src: /172.16.1.144:50010, dest: /172.16.1.104:36825, bytes: 66048, op: HDFS_READ, cliID: DFSClient_-541008184, srvID: DS-824630517-172.16.1.144-50010-1234487017305, blockid: blk_-2994819911287448111_787295


class SysLogServ(object):
    
    def __init__(self, port=5679):
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
        self.clientregex = re.compile('.*<\d+>([\w]+\s+\d{1,2} \d{2,2}:\d{2,2}:\d{2,2}).*INFO.*DataNode\.clienttrace: src: /([\d\./]+):\d+, dest: /([\d\./]+):\d+, bytes: (\d+), op: (\w+)')
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
            if self.hostnames.has_key(src):
                prettySrc = self.hostnames[src]
            else:
		try:
			prettySrc = gethostbyaddr(src)[0]
		except:
			prettySrc = src
		self.hostnames[src] = prettySrc
                print "Adding %s as %s" % (src, prettySrc)

            if self.hostnames.has_key(dest):
                prettyDest = self.hostnames[dest]
            else:
		try:
			prettyDest = gethostbyaddr(dest)[0]
		except:
			prettyDest = dest
		self.hostnames[dest] = prettyDest
                print "Adding %s as %s" % (dest, prettyDest)
            self.SendToConnected(self.connlist, "recvblock", prettySrc, prettyDest)
        elif client_match:
            time, src, dest, bytes, op = client_match.groups()
            if self.hostnames.has_key(src):
                prettySrc = self.hostnames[src]
            else:
		try:
			prettySrc = gethostbyaddr(src)[0]
		except:
			prettySrc = src
		self.hostnames[src] = prettySrc

            if self.hostnames.has_key(dest):
                prettyDest = self.hostnames[dest]
            else:
		try:
			prettyDest = gethostbyaddr(dest)[0]
		except:
			prettyDest = dest
		self.hostnames[dest] = prettyDest
                print "Adding %s as %s" % (dest, prettyDest)
                print "Adding %s as %s" % (src, prettySrc)

            self.SendToConnected(self.connlist, "clienttrace", prettySrc, prettyDest)
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
		try:
			src = gethostbyaddr(gridftparr[0])[0]
		except:
			src = gridftparr[0]
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




def main():
    child_pid = os.fork()
    if child_pid == 0:
        print os.getpid()
        fileid = open("/var/run/hadoopviz.pid", mode='w')
        fileid.write(repr(os.getpid()))
        fileid.write('\n')
        fileid.close()
    else:
        sys.exit(0);

    sysserv = SysLogServ()

    try:
        sysserv.Start()
    except KeyboardInterrupt:
        sysserv.Close()


if __name__ == "__main__":
    main()


