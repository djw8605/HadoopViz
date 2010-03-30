/*
 * DropListener.cpp
 *
 *  Created on: Jun 16, 2009
 *      Author: dweitzel
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>
#include <cstdlib>
#include <netdb.h>



#include "DropListener.h"
#include "Drops.h"
#include "Math/Math.h"
#include "HadoopDropper/IPtoLocation.h"
#include "config.h"
#include "HadoopDropper/SingleDrop.h"
#include "display.h"

extern char* serverHost;
extern short serverPort;



#define PORT_NUMBER 9345

static int CheckReadable(int sock) {

        fd_set fdread;
        FD_ZERO(&fdread);
        FD_SET(sock, &fdread);
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int socketsRetained = select(sock+1, &fdread, 0, 0, &timeout);
        FD_ZERO(&fdread);
        return socketsRetained;

}

DropListener::DropListener()
{


	this->InitializeConnection();

}

DropListener::~DropListener()
{
    // TODO Auto-generated destructor stub
    close(this->sock);

}

int DropListener::InitializeConnection()
{

    hostent* server = gethostbyname(serverHost);
    if(!server)
    {
        printf("Error looking up host: %s\nExiting...\n", serverHost);
        exit(-1);
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serv_addr.sin_family = AF_INET;

    memcpy(&serv_addr.sin_addr, server->h_addr_list[0], server->h_length);

    serv_addr.sin_port=htons(serverPort);
    if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)))
    {
    	printf("Unable to connect to %s\nTrying again...\n", serverHost);
    	return 1;

    }
    srand ( (unsigned) time ( NULL ));
    return 0;
    //bind(sock, (struct sockaddr* ) &serv_addr, sizeof(sockaddr_in));

}


void DropListener::GetDrops(deque<SingleDrop*>* s)
{
    //struct sockaddr_in in_addr;
    //int len = sizeof(struct sockaddr_in);

	static float decCounter = 0.0;
	if(decCounter >= 0.04)
	{
		_iploc->ExpDecayLoads();
		decCounter = 0.0;
	}
	else
		decCounter += getTime();

    int packlen = 0;
    int offset = 0;
    const int bufSize = 4096;
    char buf[bufSize];
    buf[bufSize-1]='\0';
    SingleDrop* sd;
    while (CheckReadable(sock))
    {

        packlen = recv(sock, buf+offset, bufSize-1, 0);
        if(packlen == 0)
        {
            printf("received close signal, was not ready for close\n");
            printf("Attempting to reconnect\n");
            close(this->sock);
            if(this->InitializeConnection())
            	return;

            //printf("received odd thing\n");

        }

        buf[packlen] = '\0';
        //printf("packlen = %i, Got a packet: %s", packlen, buf);
        char* endPos = 0;

        /* Parse the incoming stream */
        while(strchr(buf+offset, '\n') != 0)
        {
            endPos = strchr(buf+offset, '\n');
            //if((buf+offset))
            //printf("*%s*\n", buf+offset);

            /* char* endofType = strchr(buf+offset, ' '); */
            char* endofType = endPos;
            //int size = endofType - (buf+offset);

            //printf("src: %s, dest: %s\n", bufsType.src, bufsType.dest);

            /* Determine the type */
            if(strncmp(buf+offset, "packet", 6) == 0)
            {
            	//printf("Got Packet\n");
                TypeInfo bufsType = GetTypeInfo(buf+offset, endofType - buf);
                /* Create and fill the data structure */
                sd = new SingleDrop();
                IPandLoc src = _iploc->GetByIP(bufsType.src, 1.0);
                //src.AddLoad(1.0);
                IPandLoc dest = _iploc->GetByIP(bufsType.dest, 1.0);
                //dest.AddLoad(1.0);

                sd->dest = dest.GetPoint(); //_iploc->GetLocation(bufsType.dest);
                sd->src = src.GetPoint(); //_iploc->GetLocation(bufsType.src);
                sd->pos[0] = sd->src.x;
                sd->pos[1] = sd->src.y;
                sd->pos[2] = sd->src.z;
                sd->dist = Distance(sd->src, sd->dest);
                sd->scale =3;
                //sd->type = RECV_BLOCK;
                sd->type = PACKET;
                sd->counter = 0;
                sd->direction = ToVector(sd->src, sd->dest);

                free(bufsType.dest);
                free(bufsType.src);
                //Normalize(sd->direction);
                (*s).push_back(sd);


            }
            else if (strncmp(buf+offset, "ssh", 3) == 0)
            {
				//printf("Get ssh\n");
                TypeInfo bufsType = GetTypeInfo(buf + offset, endofType - buf);

                /* Create and fill the data structure */
                sd = new SingleDrop();
                IPandLoc src = _iploc->GetByIP(bufsType.src, 1.0);
				//src.AddLoad(1.0);
				IPandLoc dest = _iploc->GetByIP(bufsType.dest, 1.0);
				//dest.AddLoad(1.0);
				//printf("%lf\n", dest.GetLoad());
				sd->dest = dest.GetPoint(); //_iploc->GetLocation(bufsType.dest);
				sd->src = src.GetPoint(); //_iploc->GetLocation(bufsType.src);
				sd->pos[0] = sd->src.x;
				sd->pos[1] = sd->src.y;
				sd->pos[2] = sd->src.z;
				sd->dist = Distance(sd->src, sd->dest);
                sd->scale = ((float)rand() / RAND_MAX)*1.0+3;
                //printf("scale: %lf", sd->scale);
                //sd->scale = 3;
                if (strcmp(bufsType.extra, "Accepted") == 0)
                	sd->type = SSH_ACCEPT;
                else
                	sd->type = SSH_DENY;
                sd->counter = 0;
                sd->direction = ToVector(sd->src, sd->dest);
                //printf("SSH: %s\n", bufsType.extra);

				free(bufsType.extra);
                free(bufsType.dest);
                free(bufsType.src);
                (*s).push_back(sd);
				//Normalize(sd->direction);
				//printf("src: %lf, %lf, %lf\n", sd->direction.x, sd->direction.y, sd->direction.z);

			} else if (strncmp(buf + offset, "globus", 6) == 0) {

				//printf("Get ssh\n");
				TypeInfo bufsType = GetTypeInfo(buf + offset, endofType - buf);

				/* Create and fill the data structure */
				sd = new SingleDrop();
				IPandLoc src = _iploc->GetByIP(bufsType.src, 1.0);
				//src.AddLoad(1.0);
				IPandLoc dest = _iploc->GetByIP(bufsType.dest, 1.0);
				//dest.AddLoad(1.0);
				//printf("%lf\n", dest.GetLoad());
				sd->dest = dest.GetPoint(); //_iploc->GetLocation(bufsType.dest);
				sd->src = src.GetPoint(); //_iploc->GetLocation(bufsType.src);
				sd->pos[0] = sd->src.x;
				sd->pos[1] = sd->src.y;
				sd->pos[2] = sd->src.z;
				sd->dist = Distance(sd->src, sd->dest);
				sd->scale = ((float) rand() / RAND_MAX) * 1.0 + 3;
				//printf("scale: %lf", sd->scale);
				//sd->scale = 3;
				sd->type = GLOBUS;
				sd->counter = 0;
				sd->direction = ToVector(sd->src, sd->dest);

				free(bufsType.dest);
				free(bufsType.src);
				(*s).push_back(sd);
				//Normalize(sd->direction);
				//printf("src: %lf, %lf, %lf\n", sd->direction.x, sd->direction.y, sd->direction.z);



            }
            else if (strncmp(buf + offset, "float", 5) == 0)
            {

                TypeInfo bufsType = GetTypeInfo(buf + offset, endofType - buf);

                /* Create and fill the data structure */
                sd = new SingleDrop();
                //sd->src = _iploc->GetLocation(bufsType.src);
                IPandLoc src = _iploc->GetByIP(bufsType.src, 1.0);
                sd->src = src.GetPoint();
                point p;
                p.x = sd->src.x;
                p.y = sd->src.y;
                p.z = sd->src.z + DROP_HEIGHT;
                sd->dest = p;
                sd->pos[0] = sd->src.x;
                sd->pos[1] = sd->src.y;
                sd->pos[2] = sd->src.y;
                sd->dist = Distance(sd->src, sd->dest);
                //sd->scale = ((float) rand() / RAND_MAX) * 1.0 + 3;
                //printf("scale: %lf", sd->scale);
                //sd->scale = 3;
                sd->type = FLOAT;
                sd->counter = 0;
                sd->direction = ToVector(sd->src, sd->dest);
                //printf("float: %lf, %lf, %lf\n", sd->direction.x, sd->direction.y, sd->direction.z);

                //free(bufsType.dest);
                //printf("doing float: %s\n", bufsType.src);

                free(bufsType.src);
                free(bufsType.dest);
                (*s).push_back(sd);

            }

            offset = (endPos - buf) + 1;



        }

        /* To account for partial messages */
        if (offset < packlen)
        {
            memcpy(buf, buf+offset, packlen-offset);
            offset = packlen-offset;
        }
        offset = 0;
    }

        /*
        sd = new Drops::single_drop;
        char* ip = strchr(buf, ' ') + 1;
        //printf("ip determined to be:%s\n", ip);
        point p = _iploc->GetLocation(strchr(buf, ' ') + 1);
        sd->pos[0] = p.x;
        sd->pos[1] = p.y;
        if (buf[0] == 'a')
        {
            sd->pos[2] = DROP_CREATION;
            sd->type = BLOCK_ADD;
        }
        else if (buf[0] == 'o')
        {
            sd->pos[2] = 0.0;
            sd->type = FILE_OPEN;
        }
        else if(buf[0] == 'd')
        {
            sd->pos[2] = DROP_CREATION;
            sd->type = BLOCK_DEL;
        }



        (*s).push_back(sd);
    }
    */
}


TypeInfo DropListener::GetTypeInfo(char* buf, int size)
{
    TypeInfo toReturn;
    toReturn.extra = 0;
    char* tmp = (char*) malloc(size+2);
    memcpy(tmp, buf, size+1);
    tmp[size+1] = '\0';
    buf = tmp;
    // Get the first ip, the string after the first space
    char* firstIP = strchr(buf, ' ') + 1;
    int nfirstIP = strchr(firstIP, ' ') - firstIP;
    toReturn.src = (char*) malloc(nfirstIP + 1);
    memcpy(toReturn.src, firstIP, nfirstIP);
    toReturn.src[nfirstIP] = '\0';

    // Check if there is a second IP
    if (((nfirstIP + firstIP) - buf) == size)
    {

        printf("%s\n%s\n", buf, toReturn.src);

        printf("there is only 1 IP\n");
        toReturn.dest = 0;

    }
    else
    {

        //second ip, after the second space
        char* secondIP = strchr(firstIP, ' ') + 1;
        int nsecondIP;
        if ((strchr(secondIP, ' ') != 0) && (strchr(secondIP, ' ') < strchr(secondIP, '\n')))
        {
        	nsecondIP = strchr(secondIP, ' ') - secondIP;
        	char* extra = nsecondIP + secondIP + 1;
        	int nExtra = strchr(extra, '\n') - extra;
        	toReturn.extra = (char*) malloc (nExtra+1);
        	memcpy(toReturn.extra, extra, nExtra);
        	toReturn.extra[nExtra] = '\0';
        }
        else
        	nsecondIP = strchr(secondIP, '\n') - secondIP;
        toReturn.dest = (char*) malloc(nsecondIP + 1);
        memcpy(toReturn.dest, secondIP, nsecondIP);
        toReturn.dest[nsecondIP] = '\0';
    }

    // Check if there is any extra text


    //printf("buf = %s\n", buf);
    //printf("first:%i, second:%i\n", nfirstIP, nsecondIP);

    //new char[nfirstIP] + 1;
    //new char[nsecondIP] + 1;

    free(tmp);
    return toReturn;


}

