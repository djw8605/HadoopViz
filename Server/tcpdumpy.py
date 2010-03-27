#!/usr/bin/python



import subprocess
import sys,re,select,time


from socket import *
line_re = re.compile(".*IP (\d+.\d+.\d+.\d+).*> (\d+.\d+.\d+.\d+)")
host = "129.93.229.173"


def ParseLine(line):
   regex_line = line_re.search(line)
   if regex_line:
      return regex_line.groups()


sock = socket(AF_INET, SOCK_DGRAM)

def SendParsed(parsed):
   if host not in parsed:
      sock.sendto("packet " + " ".join(parsed), (host, 9345))
#      print " ".join(parsed)

def ReadOutput(tcpdump_out):
   while 1:
      if len(select.select( [tcpdump_out], [], [], 0.0)[0]) != 0:
         parsed = ParseLine(tcpdump_out.readline())
         if parsed != None:
            SendParsed(parsed)
      else:
         time.sleep(.01)


def main():
   tcpdump_proc = subprocess.Popen("/usr/sbin/tcpdump -n -i eth0", stdout=subprocess.PIPE, shell=True, stderr=subprocess.PIPE)
   tcp_stdout = tcpdump_proc.stdout
   tcp_stderr = tcpdump_proc.stderr
   while len(select.select( [tcp_stderr], [], [], 0.0)[0]) != 0:
      print tcp_stderr.readline()

   ReadOutput(tcp_stdout)


if __name__ == "__main__":
   main()

