#!/usr/bin/python



import subprocess
import sys,re,select,time


from socket import *
line_re = re.compile(".*IP (\d+.\d+.\d+.\d+).([\d|\w]+)\s+>\s+(\d+.\d+.\d+.\d+).([\d|\w]+)")
host = "129.93.229.173"


class TCPParser:
    def __init__(self, interface):
        self.process = None
        self.interface = interface
        self.incriment = 21600
        self.sock = socket(AF_INET, SOCK_DGRAM)
        
    def Start(self):
        self.CreateProcess()
        self.ReadOutput()
        pass

    def CreateProcess(self):
        global host
        self.tcpdump_proc = subprocess.Popen("/usr/sbin/tcpdump -tn -i %s ip host not %s" % (self.interface, host), stdout=subprocess.PIPE, shell=True, stderr=subprocess.PIPE)
        self.tcp_stdout = self.tcpdump_proc.stdout
        self.tcp_stderr = self.tcpdump_proc.stderr
        while len(select.select( [self.tcp_stderr], [], [], 0.0)[0]) != 0:
            print self.tcp_stderr.readline()
        
    def DestroyProcess(self):
        self.tcpdump_proc.terminate()
        self.tcpdump_proc.kill()
        
    def ParseLine(self, line):
        global line_re
        regex_line = line_re.search(line)
        if regex_line:
            return regex_line.groups()

    def SendParsed(self, parsed):
        global host
        #if host not in parsed:
        self.sock.sendto("packet " + " ".join( ( parsed[0], parsed[2], parsed[1], parsed[3] ) ), (host, 9345))
        #print " ".join( (parsed[0], parsed[2], parsed[1], parsed[3]) )

    def ReadOutput(self):
        started = time.time()
        while 1:
            if len(select.select( [self.tcp_stdout], [], [], 0.0)[0]) != 0:
                parsed = self.ParseLine(self.tcp_stdout.readline())
                if parsed != None:
                    self.SendParsed(parsed)
                else:
                    time.sleep(.01)
            if (time.time() - started) > self.incriment:
                started = time.time()
                self.DestroyProcess()
                time.sleep(.1)
                self.CreateProcess()

# /usr/sbin/tcpdump -t -n -i eth1 ip host not 129.93.229.226
def main():
    interface = "eth0"
    tcpparser = TCPParser(interface)
    tcpparser.Start()
   

   


if __name__ == "__main__":
   main()

