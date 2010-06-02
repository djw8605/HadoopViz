/*
 * Drops.cpp
 *
 *  Created on: Jun 16, 2009
 *      Author: dweitzel
 */

//#include <GL/glut.h>
#include <deque>
#include <cmath>
#include <SDL.h>
#include <SDL_opengl.h>

#include "display.h"
#include "HadoopDropper/DropListener.h"
#include "Drops.h"
#include <stdio.h>
#include "defaults.h"


#ifdef USE_VBO
#include "ModelLoader/CModel3DS.h"
#endif

#include <deque>

#define INITIAL_DROPS 1000
#define DROP_SPEED 1.0

Drops::Drops()
{
    // TODO Auto-generated constructor stub
/*    Drops::single_drop* sd;

    for(int i = 0;  i < 1000; i ++)
    {
        sd = new Drops::single_drop;
        sd->pos[2] = 30;
        sd->pos[1] = (float)i;
        sd->pos[0] = (float)i;
        m_drops.push_front(sd);
    }
    printf("before new\n");
*/    m_dl = new DropListener();
    m_dropList = 0;
    //printf("after new\n");

#ifdef USE_VBO
    m_VBOSupported = IsExtensionSupported((char*)"GL_ARB_vertex_buffer_object");

    if (m_VBOSupported)
    {
        m_raindrop = new CModel3DS("Media/raindrop.3ds");
        m_raindrop->CreateVBO();
        printf("VBOs supported");
    }
    else
    {
        printf("VBOs not supported");

    }
#else
    m_VBOSupported = false;
#endif //USE_VBO

    this->InitDisplayLists();

}

Drops::~Drops()
{
    // TODO Auto-generated destructor stub

#ifdef USE_VBO
    if(m_VBOSupported)
        delete m_raindrop;
#endif
    delete m_dl;
    glDeleteLists(m_dropList, 1);
}

void Drops::InitDisplayLists() {


		m_dropList = glGenLists(1);
		glNewList(m_dropList, GL_COMPILE);
		glBegin( GL_TRIANGLES);

		//bottom triangle 1/4
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 1.0);
		glVertex3f(0.0, 1.0, 1.0);

		//bottom triangle 2/4
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 1.0);
		glVertex3f(-1.0, 0.0, 1.0);

		//bottom triangle 3/4
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(-1.0, 0.0, 1.0);
		glVertex3f(0.0, -1.0, 1.0);

		//bottom triangle 4/4
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, -1.0, 1.0);
		glVertex3f(1.0, 0.0, 1.0);
		///////////////////
		// top triangle 1/4
		glVertex3f(0.0, 0.0, 4.0);
		glVertex3f(1.0, 0.0, 1.0);
		glVertex3f(0.0, 1.0, 1.0);

		//top triangle 2/4
		glVertex3f(0.0, 0.0, 4.0);
		glVertex3f(0.0, 1.0, 1.0);
		glVertex3f(-1.0, 0.0, 1.0);

		//top triangle 3/4
		glVertex3f(0.0, 0.0, 4.0);
		glVertex3f(-1.0, 0.0, 1.0);
		glVertex3f(0.0, -1.0, 1.0);

		//top triangle 4/4
		glVertex3f(0.0, 0.0, 4.0);
		glVertex3f(0.0, -1.0, 1.0);
		glVertex3f(1.0, 0.0, 1.0);

		glEnd();
		glEndList();



}


void Drops::Render()
{
    //printf("before get drops\n");
    //fflush(stdout);




    m_dl->GetDrops(&m_drops);
    //printf("after get drops\n");

    /* If there are no drops, don't do anything */
    if(m_drops.size() == 0)
    	return;

    glPushMatrix();
    deque<SingleDrop*>::iterator i;
    //printf("first element: %lf\n", m_drops.front()->counter);
    int counter = 0;
    i = m_drops.begin();
    //printf("Size = %i\n", m_drops.size());
    //SingleDrop* fr = m_drops.front();
    //while( (i != m_drops.end() ) && ((*i)->counter > 1.0))
    while(  (m_drops.size() > 0) && (m_drops.front()->counter >= 1.0))
    {
        //Drops::single_drop* s = m_drops.front();

        SingleDrop* s = m_drops.front();
        //printf("deleting = %p\nsize=%i", s, m_drops.size());
        //free(*i);
        //printf("s->pos: %lf, %lf %lf\n", s->pos[0], s->pos[1], s->pos[2]);

        delete s;
        m_drops.pop_front();
        //delete s;
        //delete m_drops.front();
        //m_drops.pop_front();
        counter++;
    }

    /*if( (*i) != m_drops.front() )
    {
        i--;
        m_drops.erase(m_drops.begin(), i);
    }
*/
   /* if(counter)
        printf("Deleted: %i\n", counter);
        */
    for (i = m_drops.begin(); i != m_drops.end(); ++i)
    {
        /*if((*i)->counter > 1.0)
            printf("why no delete???\n");*/
        RenderDrop((*i));
    }


    glPopMatrix();


}


void Drops::RenderDrop(SingleDrop* s)
{
    /*if ((s->pos[2] < 0.0) || (s->pos[2] > DROP_CREATION)) {
        delete m_drops.front();
        m_drops.pop_front();

        return;
    }
*/
    /*if((s->counter > 1))
    {

        printf("deleting\n");
        if(s != m_drops.front())
        {
            printf("front is not right now\n");
        }
        fflush(stdout);
        //delete s;
        Drops::single_drop* blah = m_drops.front();
        Drops::single_drop* blah2 = m_drops[0];
        m_drops.pop_front();
        delete m_drops.front();
        printf("after deleting\n");
        fflush(stdout);
        m_drops.pop_front();

        return;
    }
*/

    glPushMatrix();

    /* Movement parts */
    if((s->type == BLOCK_ADD) || (s->type == BLOCK_DEL))
        s->pos[2] = s->pos[2] - getTime() * DROP_SPEED;
    else if (s->type == FILE_OPEN)
    {
        s->pos[2] = s->pos[2] + getTime() * DROP_SPEED;
    }
    else if (s->type == CLIENT_TRACE)
    {

        s->counter = s->counter + (getTime() * DROP_SPEED);
        if(s->counter >= 1.0)
        	s->counter = 1.0;
        //printf("counter=%lf\n", s->counter);
        s->pos[0] = (s->direction.x * s->counter) + s->src.x;
        s->pos[1] = (s->direction.y * s->counter) + s->src.y;
        /*if((s->pos[0] < 0.0) || (s->pos[1] < 0.0))
        {
            printf("less than 0\n");
            printf("********\nsrc: %lf, %lf, %lf\ndest: %lf, %lf, %lf\ndirection: %lf, %lf, %lf\n", s->src.x, s->src.y, s->src.z, s->dest.x, s->dest.y, s->dest.z, s->direction.x,s->direction.y,s->direction.z);
            printf("pos: %lf, %lf, %lf", s->pos[0],s-> pos[1], s->pos[2]);
        }
        */
        s->pos[2] = ( - ( pow( ((2.0/s->dist) * (s->counter*s->dist) ) - 1.0, 2) )+1.0) * (s->dist/2);



    }
    else if((s->type == DROP) || (s->type==FLOAT))
    {
        s->counter = s->counter + (getTime() * DROP_SPEED);
        if (s->counter >= 1.0)
            s->counter = 1.0;


        s->pos[2] = s->src.z + (s->direction * s->counter).z;

        //printf("z=%lf\n", s->direction.z);

        /*
        if (s->type == DROP)
        {
            s->pos[2] = s->src.z - (s->counter * s->dist);
        }
        else if (s->type == FLOAT)
        {
            s->pos[2] = s->src.z + (s->counter * s->dist);
        }*/

    }

    glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    //printf("%lf\n", s->scale);
    if (s->type == CLIENT_TRACE)
    	glScalef(s->scale, s->scale, s->scale);
    else
        glScalef(0.1, 0.1, 0.1);

    /* Invert the raindrop */
    if(s->type == FILE_OPEN || s->type == FLOAT)
    {
        glScalef(1.0, 1.0, -1.0);
    }

#ifdef USE_VBO
    if (m_VBOSupported)
        m_raindrop->Draw();
    else
#endif
    {

    	glCallList(m_dropList);


    }
    //glutSolidSphere(1.0, 5, 5);


    glPopMatrix();

}
