/*
 * Cursor.cpp
 *
 *  Created on: Jun 18, 2009
 *      Author: dweitzel
 */

#include "Cursor/Cursor.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include <cstdio>

D_Cursor::D_Cursor()
{



}

D_Cursor::~D_Cursor()
{



}


D_Cursor* D_Cursor::GetInstance()
{

    static D_Cursor* inst = new D_Cursor();
    return inst;



}


void D_Cursor::Render()
{
    //printf("%lf, %lf, %lf\n", m_p.x, m_p.y, m_p.z);
    //glPushMatrix();
    //glScalef(10.0, 10.0, 10.0);
    //glTranslatef(m_p.x, m_p.y, m_p.z);
    //glColor4f(1.0, 1.0, 0.0, 0.3);
    //glBegin(GL_QUADS);
    //glVertex3f(-1.0, -1.0, 0.1);
    //glVertex3f(-1.0, 1.0, 0.1);
    //glVertex3f(1.0, 1.0, 0.1);
    //glVertex3f(1.0, -1.0, 0.1);
    //glEnd();
    //glPopMatrix();

}

void D_Cursor::SetScreenCoords(point p)
{
    m_p = p;
}

