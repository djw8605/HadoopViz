#!/usr/bin/python


import sys
import optparse
from socket import *


def main():
   udpsock = socket(AF_INET, SOCK_DGRAM)
   udpsock.bind( ('', 9345) )
   while True:
      data, addr = udpsock.recvfrom(2048)
      print data


if __name__=="__main__":
   main()




