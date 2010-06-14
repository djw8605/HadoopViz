/*
 * DropListener.h
 *
 *  Created on: Jun 16, 2009
 *      Author: dweitzel
 */

#ifndef DROPLISTENER_H_
#define DROPLISTENER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <deque>

#include "HadoopDropper/SingleDrop.h"

struct TypeInfo
{
    char src[64];
    char dest[64];
    char extra[64];

};

using namespace std;
class DropListener
{
public:
    DropListener();
    virtual ~DropListener();
    void GetDrops(deque<SingleDrop*>* s);

private:
    TypeInfo GetTypeInfo(char* buf, int size);
    void InitializeConnection();

    int sock;
    struct sockaddr_in serv_addr;
};

#endif /* DROPLISTENER_H_ */
