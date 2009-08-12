#!/usr/bin/python

from socket import *
from select import *


class EventDispatchServer:
	
	def __init__(self, port=5678):
		self.host = ''
		self.port = port
		self.addr = (self.host,self.port)
		self.buf = 1024
		self.udpsock = socket(AF_INET, SOCK_DGRAM)
		self.tcpsock = socket(AF_INET, SOCK_STREAM)


	def Start(self):

		self.tcpsock.bind(self.addr)
		self.tcpsock.listen(3)
		self.udpsock.bind(self.addr)

		

		connlist = []
		while 1:
			selectList = []
			selectList.append(self.tcpsock)
			selectList.append(self.udpsock)
			for con in connlist:
				selectList.append(con)
			results = select(selectList, [], [])
			for con in connlist:
				if results[0].count(con) is not 0:
					print "closing connection"
					con.close()
					connlist.remove(con)
			if results[0].count(self.tcpsock) is not 0:
				print "incomming connection"
				consoc, conaddr = self.tcpsock.accept()
				connlist.append(consoc)
			if results[0].count(self.udpsock) is not 0:
				print "incomming udp packet"
				data, addr = self.udpsock.recvfrom(self.buf)
				print data
				for sock in connlist:
					sock.send(data)
		


def main():
	eds = EventDispatchServer()
	eds.Start()


if __name__ == "__main__":
	main()

