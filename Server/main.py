#!/usr/bin/python

import time, os, re
from socket import *

iptosend = '129.93.229.130'
porttosend = 5678
addr = (iptosend, porttosend)
udpsock = socket(AF_INET, SOCK_DGRAM)

def sendPacket(type, ip):
    data = "%s %s" % ( type, ip )
    udpsock.sendto(data, addr)

filename = '/scratch/hadoop/logs/hadoop-root-namenode-hadoop-name.log'
file = open(filename, 'r')

st_results = os.stat(filename)
st_size = st_results[6]
file.seek(st_size)

addregex = re.compile('addStoredBlock')
openregex = re.compile('cmd=open')
deleteregex = re.compile('to delete')

ipregex = re.compile('ip=')
ipaddregex = re.compile('updated:')
delipregex = re.compile('ask')

while 1:
   where = file.tell()
   line = file.readline()
   if not line:
      time.sleep(0.01)
      file.seek(where)
   else:
      if addregex.search(line):
         pos = ipaddregex.search(line).end()+1
         ip = line[pos:line.index(':', pos)]
         sendPacket('add', ip)
      elif openregex.search(line):
         pos = ipregex.search(line).end()
         pos += 1
         ip = line[pos:line.index('\t', pos)]
         sendPacket('open', ip)
      elif deleteregex.search(line):
         pos = delipregex.search(line).end()
         pos += 1
         ip = line[pos:line.index(':', pos)]
         sendPacket('del', ip)


