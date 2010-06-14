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
#include "defaults.h"
#include "HadoopDropper/SingleDrop.h"
#include "display.h"

extern char* serverHost;
extern short serverPort;


#define PORT_NUMBER 5679

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

void DropListener::InitializeConnection()
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
    	printf("Unable to connect to %s\nNothing to do, Exiting...\n", serverHost);
    	exit(-1);

    }
    srand ( (unsigned) time ( NULL ));
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
    static float max_size = 0;
    while (CheckReadable(sock))
    {

        packlen = recv(sock, buf+offset, bufSize-1, 0);
        if(packlen == 0)
        {
            printf("received close signal, was not ready for close");
            printf("Attempting to reconnect\n");
            close(this->sock);
            this->InitializeConnection();

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
            if(strncmp(buf+offset, "recvblock", 9) == 0)
            {

                TypeInfo bufsType = GetTypeInfo(buf+offset, endofType - buf);
                /* Create and fill the data structure */
                sd = new SingleDrop();
                IPandLoc & src = _iploc->GetByIP(bufsType.src, 1.0);
                //src.AddLoad(1.0);
                IPandLoc & dest = _iploc->GetByIP(bufsType.dest, 1.0);
                //dest.AddLoad(1.0);

                sd->dest = dest.GetPoint(); //_iploc->GetLocation(bufsType.dest);
                sd->src = src.GetPoint(); //_iploc->GetLocation(bufsType.src);
                sd->pos[0] = sd->src.x;
				sd->pos[1] = sd->src.y;
				sd->pos[2] = sd->src.z;
				sd->dist = Distance(sd->src, sd->dest);
				float size;
				sscanf(bufsType.extra, "%f", &size);
				if (size * 0.9 > max_size)
				{
					max_size = size * 0.9;
					//printf("Max Size = %lf\n", max_size);
				}
				sd->scale = max(min((double) ((float) size / ((float) size + max_size)), 0.8), 0.2);
				//sd->type = RECV_BLOCK;
				sd->type = CLIENT_TRACE;
				sd->counter = 0;
				sd->direction = ToVector(sd->src, sd->dest);

                //free(bufsType.dest);
                //free(bufsType.src);
                //free(bufsType.extra);
                //Normalize(sd->direction);
                (*s).push_back(sd);


            }
            else if (strncmp(buf+offset, "clienttrace", 11) == 0)
            {

                TypeInfo bufsType = GetTypeInfo(buf + offset, endofType - buf);

                /* Create and fill the data structure */
                float size;
                sscanf(bufsType.extra, "%f", &size);
                sd = new SingleDrop();
                IPandLoc & src = _iploc->GetByIP(bufsType.src, size);
				//src.AddLoad(1.0);
				IPandLoc & dest = _iploc->GetByIP(bufsType.dest, size);
				//dest.AddLoad(1.0);
				//printf("%lf\n", dest.GetLoad());
				sd->dest = dest.GetPoint(); //_iploc->GetLocation(bufsType.dest);
				sd->src = src.GetPoint(); //_iploc->GetLocation(bufsType.src);
				sd->pos[0] = sd->src.x;
				sd->pos[1] = sd->src.y;
				sd->pos[2] = sd->src.z;
				sd->dist = Distance(sd->src, sd->dest);

				if(size*0.9 > max_size)
				{
					max_size = size*0.9;
					//printf("Max Size = %lf\n", max_size);
				}
				sd->scale = max(min((double)((float)size/((float)size + max_size)), 0.8), 0.2);
				if ( (sd->scale > 1.0) || (sd->scale < 0.0))
					printf("scale = %lf, size = %f, max_size = %lf\n", sd->scale, size, max_size);
                //sd->scale = ((float)rand() / RAND_MAX)*1.0+3;
                //printf("scale: %lf", sd->scale);
                //sd->scale = 3;
                sd->type = CLIENT_TRACE;
                sd->counter = 0;
                sd->direction = ToVector(sd->src, sd->dest);


                //free(bufsType.dest);
                //free(bufsType.src);
                //free(bufsType.extra);
                (*s).push_back(sd);
                //Normalize(sd->direction);
                //printf("src: %lf, %lf, %lf\n", sd->direction.x, sd->direction.y, sd->direction.z);

            }
            else if (strncmp(buf + offset, "drop", 4 ) == 0)
            {

                TypeInfo bufsType = GetTypeInfo(buf + offset, endofType - buf);

                /* Create and fill the data structure */
                sd = new SingleDrop();
                IPandLoc & dest = _iploc->GetByIP(bufsType.src, 1.0);
                //sd->dest = _iploc->GetLocation(bufsType.src);
                sd->dest = dest.GetPoint();
                point p;
                p.x = sd->dest.x;
                p.y = sd->dest.y;
                p.z = sd->dest.z+DROP_HEIGHT;
                sd->src =p;
                sd->pos[0] = p.x;
                sd->pos[1] = p.y;
                sd->pos[2] = p.z;
                sd->dist = Distance(sd->src, sd->dest);
                //sd->scale = ((float) rand() / RAND_MAX) * 1.0 + 3;
                //printf("scale: %lf", sd->scale);
                //sd->scale = 3;
                sd->type = DROP;
                sd->counter = 0;
                sd->direction = ToVector(sd->src, sd->dest);

                //free(bufsType.dest);
                //free(bufsType.src);
                //free(bufsType.dest);
                //free(bufsType.extra);
                (*s).push_back(sd);

            }
            else if (strncmp(buf + offset, "float", 5) == 0)
            {

                TypeInfo bufsType = GetTypeInfo(buf + offset, endofType - buf);

                /* Create and fill the data structure */
                sd = new SingleDrop();
                //sd->src = _iploc->GetLocation(bufsType.src);
                IPandLoc & src = _iploc->GetByIP(bufsType.src, 1.0);
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
                //free(bufsType.src);
                //free(bufsType.dest);
                //free(bufsType.extra);
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

    char* formatted_buf = new char[size+1];
    strncpy(formatted_buf, buf, size);
    formatted_buf[size] = '\0';
    char type[64];

    // Get the information.
    sscanf(formatted_buf, "%s %s %s %s", type, toReturn.src, toReturn.dest, toReturn.extra);
    //printf("type = %s, src_ip = %s, dst_ip = %s, extra = %s\n", type, src_ip, dst_ip, extra);

    delete [] formatted_buf;
    return toReturn;



}

