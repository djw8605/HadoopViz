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

# Sample client trace:
# <86>sshd[24552]: Accepted publickey for aswearngin from 129.93.229.160 port 42346 ssh2
# <86>sshd[24494]: Failed password for dweitzel from 129.93.165.2 port 35596 ssh2
# <86>sshd[24179]: Accepted password for dweitzel from 129.93.165.2 port 35587 ssh2
 
class SysLogServ(object):
    
    def __init__(self, probeConfig, port=9345):
        self.port = port
        self.udpservSocket = socket(AF_INET, SOCK_DGRAM)
#        self.syslogSocket =  socket(AF_INET, SOCK_DGRAM)
        self.tcpservSocket = socket(AF_INET,SOCK_STREAM)
        self.bufsize = 2048
        self.hostnames = {}
        
        self.InitRegex()
       
    def InitRegex(self):
        self.srcregex = re.compile('src:')
        self.destregex = re.compile('dest:')
        self.clientregex = re.compile('.*<\d+>([\w]+[\s]+\d{1,2} \d{2,2}:\d{2,2}:\d{2,2}).*INFO.*DataNode\.clienttrace: src: /([\d\./]+):\d+, dest: /([\d\./]+):\d+, bytes: (\d+), op: (\w+)')
        self.sshregex = re.compile(".*<\d+>.*: (\w+) (\w+) for ([\w|\d]+) from ([\d|\.]+)")
        self.packetregex = re.compile("packet ([\d|\.]+) ([\d|\.]+)")
        self.globusregex = re.compile(".*gatekeeper\[\d+\]:.*ion\s+([\d+|\.]+)")
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
#        self.syslogSocket.bind(('', syslogport))
        self.tcpservSocket.bind(('', self.port))
        self.tcpservSocket.listen(3)
        
        second = time.time()
        counter = 0
        
        self.connlist = []
        selectList = []
        self.connlist.append(self.tcpservSocket)
        self.connlist.append(self.udpservSocket)
#        self.connlist.append(self.syslogSocket)
#        self.permlist = [ self.tcpservSocket, self.udpservSocket, self.syslogSocket ]
        self.permlist = [ self.tcpservSocket, self.udpservSocket ]
        #event loop
        while 1:
            parse = None
            results = select( self.connlist, [], [])
            
            #packets per second statistics
            if int(time.time()) != second:
                print "pps: %i" % counter
                counter = 0
                second = int(time.time())
            else:
                counter = counter + 1
            
            
            #check for closing connections
            for con in results[0]:
                if self.permlist.count(con) == 0:
                    print "closing connection"
                    con.close()
                    self.connlist.remove(con)
                    
            #check for incoming connections
            if results[0].count(self.tcpservSocket) is not 0:
                print "incoming connection"
                consoc, conaddr = self.tcpservSocket.accept()
                print conaddr
                self.connlist.append(consoc)
                
            #incoming packets from syslog
            
            if results[0].count(self.udpservSocket) is not 0:
                data, addr = self.udpservSocket.recvfrom(self.bufsize)
                self.Parse(data, addr)
#            if results[0].count(self.syslogSocket) is not 0:
#                data, addr = self.syslogSocket.recvfrom(self.bufsize)
#                self.Parse(data)


# self.sshregex = re.compile(".*<\d+>.*: (\w+) (\w+) for ([\w|\d]+) from ([\d|\.]+)")
    def Parse(self, data, host):
#        client_match = self.clientregex.match(data)
        ssh_match = self.sshregex.match(data)
        if ssh_match:
            accept, method, user, src = ssh_match.groups()
            print accept + " " +  method + " " +  user + " " +  src
            self.SendToConnected(self.connlist, "ssh", src, host[0], accept)
        elif (self.packetregex.match(data)) != None:
            packet_match = self.packetregex.match(data)
            src, dest = packet_match.groups()
            #print "Packet " + src + " " + dest
            self.SendToConnected(self.connlist, "packet", src, dest)
        elif (self.globusregex.match(data)) != None:
            globus_match = self.globusregex.match(data)
            src = self.globusregex.match(data).group(1)
            dest = host[0]
            self.SendToConnected(self.connlist, "globus", src, dest)
        
                    

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
            if self.permlist.count(con) != 0:
                continue
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


