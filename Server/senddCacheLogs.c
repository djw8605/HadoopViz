#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>


#include <sys/inotify.h>


#define SRV_IP "131.225.206.147"
#define BUFLEN 2048
#define NPACK 1
#define PORT 9345

void diep(char *s)
{
  perror(s);
  exit(1);
}

void usage(char *s)
{
  exit(1);
}

     

        int get_event (int fd, const char * target)
        {
           int result = 0;
        
           ssize_t len, i = 0;
           char buff[BUFLEN] = {0};
        
           len = read(fd, buff, BUFLEN);
        
           while (i < len) {
              struct inotify_event *pevent = (struct inotify_event *)&buff[i];
              char action[81+FILENAME_MAX] = {0};
        
              if (pevent->len)
                 strcpy (action, pevent->name);
              else
                 strcpy (action, target);
        
              if (pevent->mask & IN_ACCESS)
                 strcat(action, " was read");
              if (pevent->mask & IN_ATTRIB)
                 strcat(action, " Metadata changed");
              if (pevent->mask & IN_CLOSE_WRITE) {
                 strcat(action, " opened for writing was closed");
                 result = 1;
              }
              if (pevent->mask & IN_CLOSE_NOWRITE)
                 strcat(action, " not opened for writing was closed");
              if (pevent->mask & IN_CREATE)
                 strcat(action, " created in watched directory");
              if (pevent->mask & IN_DELETE)
                 strcat(action, " deleted from watched directory");
              if (pevent->mask & IN_DELETE_SELF)
                 strcat(action, "Watched file/directory was itself deleted");
              if (pevent->mask & IN_MODIFY) {
                 strcat(action, " was modified");
                 result = 1;
              }
              if (pevent->mask & IN_MOVE_SELF)
                 strcat(action, "Watched file/directory was itself moved");
              if (pevent->mask & IN_MOVED_FROM)
                 strcat(action, " moved out of watched directory");
              if (pevent->mask & IN_MOVED_TO)
                 strcat(action, " moved into watched directory");
              if (pevent->mask & IN_OPEN)
                 strcat(action, " was opened");
        
              // printf ("%s\n", action);
        
              i += sizeof(struct inotify_event) + pevent->len;
        
           }
        
           return result;
        
        }  /* get_event */
        


int main(int argc, char **argv)
{
 struct sockaddr_in si_other;
 int s, i, slen=sizeof(si_other);
 char* buf = NULL;
 char* logfile = NULL;
 long lastseen = 0;


          const char* short_options = "hl:z:";
          static struct option long_options[] =
            {
              {"help", no_argument, 0, 'h'},
              {"logfile", required_argument, 0, 'l'},
              {"text", required_argument, 0, 'z'},
              {0, 0, 0, 0}
            };


          while(1)
          {
            int option_index = 0;
            int c = getopt_long(argc, argv, short_options, long_options, &option_index);

           /* End of options?? */
            if(c == -1)
              break;

            switch(c)
	    {
            case 'h':
              usage(argv[0]);
              exit(0);
        
            case 'z':
              buf = new char[strlen(optarg)+1];
	      strcpy(buf, optarg);
              buf[strlen(buf)] = '\0';
              break;

            case 'l':
	      FILE *pf; 
              if(logfile)
                delete logfile;
              logfile = new char[strlen(optarg)+1];
              strcpy(logfile, optarg);
              logfile[strlen(optarg)] = '\0';

	      pf = fopen(logfile,"r"); 
	      if (pf != NULL) { 
                fseek(pf,0,SEEK_END); 
                lastseen=ftell(pf);
		fclose(pf);
              } else { 
		printf ("Cannot open file %s: %d, %s\n", logfile, errno,strerror(errno));
	        usage(argv[0]);
              } 
              break;

            default:
              usage(argv[0]);
              exit(-1);
              break;
        
            }

          }


  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }



  if (logfile != NULL) { 
          int fd, wd;
          fd = inotify_init();
          if (fd < 0) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            return 1;
          }
        
          wd = inotify_add_watch(fd, logfile, IN_ALL_EVENTS);
          if (wd < 0) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
          }

	  buf=new char[BUFLEN];
	  while (1) { 
            if (get_event(fd, logfile)) {
	      FILE* pf = fopen(logfile,"r");
              if (pf != NULL) {
		fseek(pf,lastseen,SEEK_SET);
		i = fread (buf, 1, BUFLEN, pf);
                buf[i]='\0';
                fseek(pf,0,SEEK_END);
                lastseen=ftell(pf);
                fclose(pf);
              } 
	      if (strstr(buf,"pool:") || strstr (buf,"door:GFTP")) {
                printf("Sending packet %d: %s", i, buf);
      	        if (sendto(s, buf, BUFLEN, 0, (sockaddr*)&si_other, slen)==-1)
		  diep("sendto()");
              } 
	    } 
          } 

  } else { 
    for (i=0; i<NPACK; i++) {
      printf("Sending packet %d: %s\n", i, buf);
      if (sendto(s, buf, BUFLEN, 0, (sockaddr*)&si_other, slen)==-1)
        diep("sendto()");
    }
  } 



  close(s);
  return 0;
}

