
from socket import *
from random import *
import time

class TestServer:

   def __init__(self, host='derek.unl.edu', port=514):
      self.remotehost = host
      self.remoteport = port

   def Start(self):
      addr = (self.remotehost, self.remoteport)
      udpsock = socket(AF_INET, SOCK_DGRAM)
      while 1:
         if randint(0,1) is 1:
            udpsock.sendto("GRIDFTP %i" % randint(0,133), addr)

         time.sleep(0.1)
      


if __name__ == "__main__":
   ts = TestServer()
   ts.Start()


