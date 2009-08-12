/*
 * SingleDrop.h
 *
 *  Created on: Jun 26, 2009
 *      Author: dweitzel
 */

#ifndef SINGLEDROP_H_
#define SINGLEDROP_H_

#include "Math/Math.h"
enum DropType { BLOCK_ADD, FILE_OPEN, BLOCK_DEL, CLIENT_TRACE, RECV_BLOCK, DROP, FLOAT };

class SingleDrop
{
public:
    SingleDrop();
    virtual ~SingleDrop();


        float pos[3];
        DropType type;
        point dest;
        point src;
        d_vector direction;
        float dist, scale;
        float counter;



};

#endif /* SINGLEDROP_H_ */
