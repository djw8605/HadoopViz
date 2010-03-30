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
#include "config.h"


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
    this->m_sphereList = 0;
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
    glDeleteLists(this->m_sphereList, 1);
}


#define X .525731112119133606
#define Z .850650808352039932

static GLfloat vdata[12][3] = {
    {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
    {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
    {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};
static GLuint tindices[20][3] = {
    {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
    {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

void normalize(GLfloat *a) {
    GLfloat d=sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]/=d; a[1]/=d; a[2]/=d;
}

void drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r) {
    if (div<=0) {
        glNormal3fv(a); glVertex3f(a[0]*r, a[1]*r, a[2]*r);
        glNormal3fv(b); glVertex3f(b[0]*r, b[1]*r, b[2]*r);
        glNormal3fv(c); glVertex3f(c[0]*r, c[1]*r, c[2]*r);
    } else {
        GLfloat ab[3], ac[3], bc[3];
        for (int i=0;i<3;i++) {
            ab[i]=(a[i]+b[i])/2;
            ac[i]=(a[i]+c[i])/2;
            bc[i]=(b[i]+c[i])/2;
        }
        normalize(ab); normalize(ac); normalize(bc);
        drawtri(a, ab, ac, div-1, r);
        drawtri(b, bc, ab, div-1, r);
        drawtri(c, ac, bc, div-1, r);
        drawtri(ab, bc, ac, div-1, r);  //<--Comment this line and sphere looks really cool!
    }
}

void drawsphere(int ndiv, float radius=1.0) {
    glBegin(GL_TRIANGLES);
    for (int i=0;i<20;i++)
        drawtri(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], ndiv, radius);
    glEnd();
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

		this->m_sphereList = glGenLists(1);
		glNewList(m_sphereList, GL_COMPILE);
		drawsphere(2);
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
    s->counter = s->counter + (getTime() * DROP_SPEED);
    /* Movement parts */
    if((s->type == BLOCK_ADD) || (s->type == BLOCK_DEL))
        s->pos[2] = s->pos[2] - getTime() * DROP_SPEED;
    else if ((s->type == PACKET) || (s->type == GLOBUS) || (s->type == SSH))
    {

    	s->pos[0] = (s->direction.x * s->counter) + s->src.x;
    	s->pos[1] = (s->direction.y * s->counter) + s->src.y;
    	s->pos[2] = (s->direction.z * s->counter) + s->src.z;

    }
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



    /* Invert the raindrop */
    if(s->type == FILE_OPEN || s->type == FLOAT)
    {
        glScalef(1.0, 1.0, -1.0);
    }


    if (s->type == PACKET)
    {
    	glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
    	glColor4f(1.0, 1.0, 1.0, 1.0);
        glScalef(0.3, 0.3, 0.3);
    	glCallList(m_dropList);
        //drawsphere(5);


    }
    else if (s->type == GLOBUS)
    {
    	glColor4f(1.0, 1.0, 0.0, 1.0);
    	glBegin(GL_LINES);
    	glVertex3f(s->src.x,s->src.y, s->src.z);
    	glVertex3f(s->dest.x, s->dest.y, s->dest.z);
    	glEnd();
    	if ((s->counter > 0.1) && (s->counter < 0.9))
    	{
    		float tmpCounter = (s->counter - 0.1) / 0.8;
    		s->pos[0] = s->src.x + (tmpCounter * s->direction.x);
    		s->pos[1] = s->src.y + (tmpCounter * s->direction.y);
    		s->pos[2] = s->src.z + (tmpCounter * s->direction.z);
    		glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
    		glCallList(this->m_sphereList);

    	}

    }
    else if (s->type == SSH)
    {
    	//printf("SSH");
    	glColor4f(0.0, 1.0, 0.0, 1.0);
    	glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
    	//double angle = atan2(y2 - y1, x2 - x1) * 180 / PI;
    	float xyangle = (180/(M_PI)) * atan2((s->src.y - s->dest.y), (s->src.x - s->dest.x));
    	//float yzangle = (180/(M_PI)) * atan((s->src.z - s->dest.z) / (sqrt(pow(s->src.x - s->dest.x, 2) + pow(s->src.y - s->dest.y, 2))));
    	glRotatef(90, 1.0, 0.0, 0.0);
    	glRotatef(xyangle+180, 0.0, 1.0, 0.0);
    	d_vector xaxis, tmp;
    	xaxis.x = tmp.x = s->dest.x - s->src.x;
    	xaxis.y = tmp.y = s->dest.y - s->src.y;
    	xaxis.z = 0;

    	tmp.z = s->dest.z - s->src.z;
    	float theta = (180/M_PI) * acos( (xaxis * tmp) / (Magnitude(xaxis)*Magnitude(tmp)) );
    	if	(s->dest.z < s->src.z)
    		theta *= -1.0;
    	glRotatef(theta, 0.0, 0.0, 1.0);
    	glScalef(0.2, 0.2, 0.2);



    	freetype::print(*GetFont(), 0.0, 0.0, "SSH");




    }
    //glutSolidSphere(1.0, 5, 5);


    glPopMatrix();

}
