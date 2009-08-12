/*
 * Cursor.h
 *
 *  Created on: Jun 18, 2009
 *      Author: dweitzel
 */

#ifndef CURSOR_H_
#define CURSOR_H_

#include "Math/Math.h"

#define _cursor D_Cursor::GetInstance()

class D_Cursor
{
public:
    D_Cursor();
    virtual ~D_Cursor();
    static D_Cursor* GetInstance();
    void Render();
    void SetScreenCoords(point p);


private:

    point m_p;


};


#endif /* CURSOR_H_ */
