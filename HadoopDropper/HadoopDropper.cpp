/*
 * HadoopDropper.cpp
 *
 *  Created on: Jun 16, 2009
 *      Author: dweitzel
 */

//#include <GL/glut.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include <GL/glx.h>
#include <cmath>

#include <config.h>
#include "HadoopDropper.h"
#include "HadoopDropper/Drops.h"
#include <Camera/Camera.h>
#include "config.h"
#include "Math/Math.h"
#include "Cursor/Cursor.h"
#include "HadoopDropper/IPtoLocation.h"
#include "HadoopDropper/SelectionDisplay.h"
#include "display.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#define ROTATE_SPEED 10.0


HadoopDropper::HadoopDropper()
{
    // TODO Auto-generated constructor stub
    drops = new Drops();
    m_cursorOver = false;
    m_floorSize = 0;
    m_totalFloor = 0;
    this->m_selectHost = 0;
    this->m_selectIP = 0;
    this->m_selectedPoint = 0;
    this->m_selected = 0;
    this->m_clickedHost = 0;
}

HadoopDropper::~HadoopDropper()
{
    // TODO Auto-generated destructor stub
    delete drops;
    glDeleteLists(m_floorList, 1);
    glDeleteLists(m_floorTile, 1);
    glDeleteLists(m_totalFloor, 1);

}


void HadoopDropper::Render()
{
    //printf("before position\n");






    float buf[3];
    _camera->GetPosition(buf);
    static float savedPos[3] = {buf[0], buf[1], buf[2]};
    if (this->m_selected)
    {
    	//printf("%lf, %lf, %lf\n", buf[0] - this->m_selectedPoint->x, buf[1] - this->m_selectedPoint->y, buf[2] - this->m_selectedPoint->z);
    	_camera->SetCameraLocation(
    	    			buf[0],
    	    			buf[1],
    	    			this->m_selectedPoint->z,
    	    			this->m_selectedPoint->x, this->m_selectedPoint->y,
    	    			this->m_selectedPoint->z);
    	/*_camera->SetCameraLocation(
    			buf[0] + ((this->m_selectedPoint->x - buf[0])/(10.0 * getTime())),
    			buf[1] + ((this->m_selectedPoint->y - buf[1])/(10.0 * getTime())),
    			buf[2] + ((this->m_selectedPoint->z - buf[2])/(10.0 * getTime())),
    			this->m_selectedPoint->x, this->m_selectedPoint->y,
    			this->m_selectedPoint->z);
    			*/
    	_camera->ModifyRotationRadius((10.0 - _camera->GetRotationRadius()) / 100.0);
    }
    else
    {
    	//printf("%lf\n", (150.0 - _camera->GetRotationRadius()) / 100.0);
    	_camera->SetCameraLocation(buf[0], buf[1], savedPos[2], 0, 0,  0);
    	_camera->ModifyRotationRadius((200.0 - _camera->GetRotationRadius()) / 100.0);

    }

    _camera->positionCamera();
    glPushMatrix();


    //printf("before drops render\n");
    drops->Render();
    RenderNodes();
    this->MouseMove(this->mouse[0], this->mouse[1]);
    //RenderFloor();

    if(this->m_cursorOver || this->m_selected)
    {
        _cursor->Render();
        RenderSelected();
        _seldisp->Render();
    }


    glPopMatrix();

}

void HadoopDropper::Init()
{
    _camera->SetCameraLocation(0.0, -100.0, 10.0, AREA_SIZE / 2, AREA_SIZE / 2,
            10.0);
    _camera->Rotate(ROTATE_SPEED);
    _camera->ModifyRotationRadius(150.0);
    //printf("after camera set\n");
    int width = SDL_GetVideoSurface()->w; //glutGet(GLUT_WINDOW_WIDTH);
    int height = SDL_GetVideoSurface()->h; //glutGet(GLUT_WINDOW_WIDTH);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    gluPerspective(FOV, (GLfloat) width / (GLfloat) height, 1, 5000.0);

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    /*glVertex3f(0.0, 0.0, 0.0);
    glVertex3f((float)AREA_SIZE, 0.0, 0.0);
    glVertex3f((float)AREA_SIZE, (float)AREA_SIZE, 0.0);
    glVertex3f(0.0, (float)AREA_SIZE, 0.0);
    */
/*
    float l_edges = (float)(AREA_SIZE+((float)SPACE_BETWEEN/2.0)) ;
    float r_edges = 0.0 - (float)SPACE_BETWEEN/2.0;
    m_floorpoints[0] = FillPoint(r_edges, r_edges, 0.0);
    m_floorpoints[1] = FillPoint(r_edges, l_edges, 0.0);
    m_floorpoints[2] = FillPoint(l_edges, l_edges, 0.0);
    m_floorpoints[3] = FillPoint(l_edges, r_edges, 0.0);
*/
    m_floorpoints[0] = FillPoint(-(float)SPACE_BETWEEN/2.0, -(float)SPACE_BETWEEN/2.0, 0.0);
    m_floorpoints[1] = FillPoint(-(float)SPACE_BETWEEN/2.0, 0.0, 0.0);
    m_floorpoints[2] = FillPoint(0.0, 0.0, 0.0);
    m_floorpoints[3] = FillPoint(0.0, -(float)SPACE_BETWEEN/2.0, 0.0);
    float halveBetween = SPACE_BETWEEN/2.0;

    /* VETEX ARRAY (for floor) */
    this->m_numVertices =  3 * pow((float)((AREA_SIZE/SPACE_BETWEEN)+1), 2.0f);
    this->m_floorVertices = new GLfloat[this->m_numVertices];
    //printf("numVertices = %i\n", this->m_numVertices);
    for(int i = 0; i < (this->m_numVertices/3); i++)
    {
        // X
        this->m_floorVertices[(i*3)] = (GLfloat) ((i / ((AREA_SIZE+SPACE_BETWEEN)/SPACE_BETWEEN)) * SPACE_BETWEEN) - halveBetween;

        // Y
        this->m_floorVertices[(i*3)+1] =(GLfloat) (i%((AREA_SIZE+SPACE_BETWEEN)/SPACE_BETWEEN) * SPACE_BETWEEN) - halveBetween;

        // Z
        this->m_floorVertices[(i*3)+2] = (GLfloat) 0.0;

        //printf("%lf, %lf, %lf\n", this->m_floorVertices[(i*3)], this->m_floorVertices[(i*3)+1], this->m_floorVertices[(i*3)+2]);

    }

    /* INDEX ARRAY (floor) */
    this->m_numTiles = pow((double)((int)AREA_SIZE/SPACE_BETWEEN),2);
    this->m_tileIndices = new GLubyte[this->m_numTiles*4];
    //printf("numTiles = %i\n", this->m_numTiles);
    int numInRow = AREA_SIZE/SPACE_BETWEEN;
    for(int i = 0; i < this->m_numTiles; i++)
    {
        int rowNum = i/numInRow;
        int tileIndex = i * 4;
        this->m_tileIndices[tileIndex] = (rowNum*numInRow+rowNum+(i%numInRow));
        this->m_tileIndices[tileIndex+1] = (rowNum*numInRow+rowNum+(i%numInRow)+1);
        this->m_tileIndices[tileIndex+2] = (((rowNum+1)*numInRow)+(rowNum+1)+(i%numInRow)+1);
        this->m_tileIndices[tileIndex+3] = (((rowNum+1)*numInRow)+(rowNum+1)+(i%numInRow));

        /*printf("%i, %i, %i, %i\n", this->m_tileIndices[tileIndex], this->m_tileIndices[tileIndex+1], this->m_tileIndices[tileIndex+2], this->m_tileIndices[tileIndex+3]);
        printf("%lf, %lf, %lf\n", this->m_floorVertices[this->m_tileIndices[tileIndex]*3], this->m_floorVertices[this->m_tileIndices[tileIndex]*3+1], this->m_floorVertices[this->m_tileIndices[tileIndex]*3+2]);
        printf("%lf, %lf, %lf\n", this->m_floorVertices[this->m_tileIndices[tileIndex+1]*3], this->m_floorVertices[this->m_tileIndices[tileIndex+1]*3+1], this->m_floorVertices[this->m_tileIndices[tileIndex+1]*3+2]);
        printf("%lf, %lf, %lf\n", this->m_floorVertices[this->m_tileIndices[tileIndex+2]*3], this->m_floorVertices[this->m_tileIndices[tileIndex+2]*3+1], this->m_floorVertices[this->m_tileIndices[tileIndex+2]*3+2]);
        printf("%lf, %lf, %lf\n\n", this->m_floorVertices[this->m_tileIndices[tileIndex+3]*3], this->m_floorVertices[this->m_tileIndices[tileIndex+3]*3+1], this->m_floorVertices[this->m_tileIndices[tileIndex+3]*3+2]);
        */
    }

    /* COLOR ARRAY (floor) */
    this->m_floorColors = new GLubyte[this->m_numVertices];
    for(int i = 0; i < this->m_numVertices/3; i++)
    {
    	this->m_floorColors[i*3] = 0;
    	this->m_floorColors[i*3+1] = 255;
    	this->m_floorColors[i*3+2] = 0;
    }





    InitDisplayLists();

}

void HadoopDropper::InitDisplayLists()
{

	glPushMatrix();
	glLoadIdentity();

	glColor4f(0.5, 0.5, 1.0, 1.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, this->m_floorVertices);
	glDrawElements(GL_QUADS, 100, GL_UNSIGNED_BYTE, this->m_tileIndices);
	glDisableClientState(GL_VERTEX_ARRAY);

    /*glDeleteLists(m_floorList, 1);
	glDeleteLists(m_floorTile, 1);
	glDeleteLists(m_totalFloor, 1);

	m_floorTile = glGenLists(1);
	m_totalFloor = glGenLists(1);

	m_floorList = glGenLists(1);
	if (!m_floorList || !m_floorTile || !m_totalFloor) {
		printf(
				"Your computer does not support displaylists (odd!!!).\nFalling back to slow rendering.");
		m_supportLists = false;
	} else {


		glNewList(m_floorList, GL_COMPILE);
		glPushMatrix();
		glBegin( GL_QUADS);
		glColor4f(0.5, 0.5, 1.0, 1.0);
		for (int i = 0; i < 4; i++)
			glVertex3f(m_floorpoints[i].x, m_floorpoints[i].y,
					m_floorpoints[i].z);

		glEnd();
		glPopMatrix();
		glEndList();
*/
		/* Floor Tile List (used by the floor list) */
		/*glNewList(m_floorTile, GL_COMPILE);
		glBegin(GL_QUADS);
		glColor4f(0.5, 0.5, 1.0, 1.0);
		glVertex3f(0.0 - ((float) SPACE_BETWEEN / 2.0), 0.0
				- ((float) SPACE_BETWEEN / 2.0), 0.0);
		glVertex3f(0.0 - ((float) SPACE_BETWEEN / 2.0), ((float) SPACE_BETWEEN
				/ 2.0), 0.0);
		glVertex3f(((float) SPACE_BETWEEN / 2.0),
				((float) SPACE_BETWEEN / 2.0), 0.0);
		glVertex3f(((float) SPACE_BETWEEN / 2.0), 0.0 - ((float) SPACE_BETWEEN
				/ 2.0), 0.0);
		glEnd();
		glEndList();
*/
		/* FloorList */
		/*int counter = 0;
		point p;
		glDeleteLists(m_totalFloor, 1);
		glNewList(m_totalFloor, GL_COMPILE);
		while (counter < _iploc->GetSize()) {
			p = _iploc->GetLocation(counter);
			glPushMatrix();

			glTranslatef(p.x, p.y, p.z);


			glCallList(m_floorTile);

			glPopMatrix();
			counter++;
			//printf("%lf, %lf, %lf\n", p.x, p.y, p.z);

		}

		glEndList();

		m_supportLists = true;
	}
	*/
	glPopMatrix();


}


void HadoopDropper::DeInit()
{

}
void HadoopDropper::Select(int x, int y)
{
	// Get the object the mouse is over
	this->m_checkselect = true;
	this->MouseMove(x, y);
	this->m_checkselect = false;

	// If the mouse is over something, focus on it
	if(this->m_selectedPoint)
		this->m_selected = true;
	else
		this->m_selected = false;
}

void HadoopDropper::MouseMove(int x, int y)
{
		this->mouse[0] = x; this->mouse[1] = y;
        glPushMatrix();
        ray r;
        GLdouble pos3D_x, pos3D_y, pos3D_z;
        //printf("inside mouse move\n");
        glLoadIdentity();
        // arrays to hold matrix information
        _camera->positionCamera();
        GLdouble model_view[16];
        glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

        GLdouble projection[16];
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        // get 3D coordinates based on window coordinates
        //printf("x = %i, y = %i\n", x, viewport[3]-y);
        gluUnProject(x, viewport[3]-y, 0.0001,
                model_view, projection, viewport,
                &pos3D_x, &pos3D_y, &pos3D_z);

        r.origin.x = (float)pos3D_x;
        r.origin.y = (float)pos3D_y;
        r.origin.z = (float)pos3D_z;

        //printf("%lf, %lf, %lf\n", r.origin.x, r.origin.y, r.origin.z);
        gluUnProject(x, viewport[3]-y, 1.00,
                 model_view, projection, viewport,
                 &pos3D_x, &pos3D_y, &pos3D_z);
        //printf("%lf, %lf, %lf\n", pos3D_x, pos3D_y, pos3D_z);

        //ray r;
        r.direction.x = (float)pos3D_x - r.origin.x;
        r.direction.y = (float)pos3D_y - r.origin.y;
        r.direction.z = (float)pos3D_z - r.origin.z;

        //printf("Ray, o = (%lf, %lf, %lf), d = (%lf, %lf, %lf)\n", r.origin.x, r.origin.y, r.origin.z, r.direction.x, r.direction.y, r.direction.z);
        float buf[3];
        _camera->GetPosition(buf);
        //printf("camera: %lf, %lf, %lf\n", buf[0], buf[1], buf[2]);


        /* Now check if you're intersectiong the floor */
        for (int i = 0; i < _iploc->GetSize(); i++) {
		point p = _iploc->GetInfo(i).GetPos();
		point clickBox[4];
		clickBox[0].x = p.x - 1.0;
		clickBox[0].y = p.y - 1.0;
		clickBox[0].z = p.z - 1.0;

		clickBox[1].x = p.x - 1.0;
		clickBox[1].y = p.y - 1.0;
		clickBox[1].z = p.z + 1.0;

		clickBox[2].x = p.x + 1.0;
		clickBox[2].y = p.y + 1.0;
		clickBox[2].z = p.z + 1.0;

		clickBox[3].x = p.x + 1.0;
		clickBox[3].y = p.y + 1.0;
		clickBox[3].z = p.z - 1.0;

		distloc dl = Intersect(r, clickBox[0], clickBox[1], clickBox[2],
				clickBox[3]);

		clickBox[0].x += 2.0; //clickBox[0].y += 2.0;
		clickBox[1].x += 2.0; //clickBox[1].y += 2.0;
		clickBox[2].x -= 2.0; //clickBox[2].y -= 2.0;
		clickBox[3].x -= 2.0; //clickBox[3].y -= 2.0;
		distloc dl2 = Intersect(r, clickBox[0], clickBox[1], clickBox[2],
				clickBox[3]);




		if (dl.distance || dl2.distance) {
			m_intercept = dl;
			//printf("Over the floor\n");
			if (_camera->IsRotating())
				_camera->StopRotate();

			m_cursorOver = true;
			_cursor->SetScreenCoords(dl.intPoint);

			if (this->m_checkselect)
			{
				printf("Setting selectedPoint\n");

				if (this->m_selectedPoint)
					delete this->m_selectedPoint;
				this->m_selectedPoint = new point;
				*m_selectedPoint = _iploc->GetInfo(i).GetPos();

				if (this->m_clickedHost)
					delete m_clickedHost;
				this->m_clickedHost = new char[strlen(this->m_selectHost) + 1];
				strcpy(this->m_clickedHost, this->m_selectHost);
				printf("%s\n", this->m_clickedHost);

			}

			/* Delete the current host */
			if (this->m_selectIP && (strcmp(this->m_selectIP, _iploc->GetIP(i))
					== 0))
				break;
			if (this->m_selectIP)
				delete[] this->m_selectHost;
			delete[] this->m_selectIP;
			this->m_selectIP = new char[strlen(_iploc->GetIP(i)) + 1];
			strcpy(this->m_selectIP, _iploc->GetIP(i));

			// For the mouse click



			/* Get the hostname of the host */
			//hostent* server = gethostbyname(this->m_selectIP);
			//struct in_addr host_addr;

			//if (!server) {
			//	printf("Error looking up host: %s", this->m_selectIP);
			//exit(-1);
			//}
			struct sockaddr_in serv_addr;
			inet_aton(this->m_selectIP, &serv_addr.sin_addr);
			serv_addr.sin_family = AF_INET;

			//memcpy(&serv_addr.sin_addr, server->h_addr_list[0],
			//		server->h_length);
			hostent* host = gethostbyaddr(&(serv_addr.sin_addr),
					sizeof(struct in_addr), AF_INET);
			if (host) {
				//printf("%s", host->h_addr_list[0]);
				this->m_selectHost = new char[strlen(host->h_name) + 1];
				strcpy(this->m_selectHost, host->h_name);
			} else {
				//herror(0);
				this->m_selectHost = new char[strlen(this->m_selectIP) + 1];
				strcpy(this->m_selectHost, _iploc->GetIP(i));
				//printf("%s\n", this->m_selectIP);

			}


			break;

		} else {
			if (!_camera->IsRotating())
				_camera->Rotate(ROTATE_SPEED);
			m_cursorOver = false;

			// For mouse click


			if (this->m_selectedPoint && this->m_checkselect)
			{
				printf("Removing selecting point\n");
				delete this->m_selectedPoint;
				this->m_selectedPoint = 0;
				delete this->m_clickedHost;
				this->m_clickedHost = 0;

			}

		}




	}


        glPopMatrix();
}


void HadoopDropper::Reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
       glLoadIdentity();
       glViewport(0, 0, (GLsizei) w, (GLsizei) h);
       //printf("Resized to: %i, %i\n", w, h);
       gluPerspective(FOV, (GLfloat) w/ (GLfloat) h, 0.1, 5000.0);


       glMatrixMode(GL_MODELVIEW);

       glMatrixMode(GL_MODELVIEW);
       glEnable(GL_NORMALIZE);
       glEnable(GL_AUTO_NORMAL);

       glEnable(GL_BLEND);
       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       glEnable(GL_DEPTH_TEST);

       /* Now, recreate the display lists (UGH!, stupid SDL problem) */
       this->InitDisplayLists();
       drops->InitDisplayLists();

}

void HadoopDropper::Reset()
{


}

void HadoopDropper::RenderNodes()
{
	int max_load = 0;
	if (_iploc->GetSize() > 0)
		max_load = _iploc->GetInfo(0).GetMaxLoad();


	for (int i = 0; i < _iploc->GetSize(); i++)
	{

		if (_iploc->GetLoad(i) <= 1)
		{
			_iploc->RemoveNode(i);
			i--;
			continue;
		}

		float load = _iploc->GetLoad(i);
		point p = _iploc->GetLocation(i);
		IPandLoc node = _iploc->GetInfo(i);
		glPushMatrix();
		glTranslatef(p.x, p.y, p.z);
		if(node.UseColor())
		{
			//printf("Use Colors: %s\n", node.GetIP());
			float *colors = node.GetColors();
			glColor4f(colors[0], colors[1], colors[1], load/max_load);
		}
		else
			glColor4f(0.0, 1.0, 1.0, load / max_load);
		glBegin(GL_QUADS);							// Start Drawing Quads
					// Bottom Face
					glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
					glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
					glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
					glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
					// Front Face
					glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
					glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
					glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
					glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
					// Back Face
					glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
					glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
					glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
					glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
					// Right face
					glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
					glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
					glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
					glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
					// Left Face
					glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
					glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
					glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
					glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad


					/* Testing */
					/*point clickBox[4];
					clickBox[0].x = -1.0; clickBox[0].y = -1.0; clickBox[0].z = -1.0;
					clickBox[1].x = -1.0; clickBox[1].y = -1.0; clickBox[1].z = 1.0;
					clickBox[2].x = 1.0; clickBox[2].y = 1.0; clickBox[2].z = 1.0;
					clickBox[3].x = 1.0; clickBox[3].y = 1.0; clickBox[3].z = -1.0;
					for (int a = 0; a < 4; a++)
						glVertex3f(clickBox[a].x, clickBox[a].y, clickBox[a].z);
					//distloc dl = Intersect(r, clickBox[0], clickBox[1],
					//		clickBox[2], clickBox[3]);

					clickBox[0].x += 2.0; //clickBox[0].y += 2.0;
					clickBox[1].x += 2.0; //clickBox[1].y += 2.0;
					clickBox[2].x -= 2.0; //clickBox[2].y -= 2.0;
					clickBox[3].x -= 2.0; //clickBox[3].y -= 2.0;
					//		distloc dl2 = Intersect(r, clickBox[0], clickBox[1],
					//						clickBox[2], clickBox[3]);
					for (int a = 0; a < 4; a++)
						glVertex3f(clickBox[a].x, clickBox[a].y, clickBox[a].z);

			*/	glEnd();								// Done Drawing Quads

		glEnd();
		glPopMatrix();


	}


}


void HadoopDropper::RenderFloor()
{

    //int counter = 0;
    point p;


    /* check if the floor size has changed */

    if(m_floorSize != _iploc->GetSize())
    {
    	/* Ok, it changed, so redo the floor */ /*
    	glDeleteLists(m_totalFloor, 1);
    	glNewList(m_totalFloor, GL_COMPILE);
    	while(counter < _iploc->GetSize())
		{
			p = _iploc->GetLocation(counter);
			glPushMatrix();

			glTranslatef(p.x, p.y, p.z);



			glCallList(m_floorTile);

			glPopMatrix();
			counter++;
			//printf("%lf, %lf, %lf\n", p.x, p.y, p.z);

		}
*/
    	/* Increase the size of the intersect area, for selection */
    	for (int i = _iploc->GetSize() - (AREA_SIZE / SPACE_BETWEEN); i
				< _iploc->GetSize(); i++) {
			p = _iploc->GetLocation(i);

			m_floorpoints[1].y = max(m_floorpoints[1].y, p.y + (SPACE_BETWEEN / 2));
			m_floorpoints[2].y = m_floorpoints[1].y;
			m_floorpoints[2].x = max(m_floorpoints[1].x, p.x + (SPACE_BETWEEN / 2));
			m_floorpoints[3].x = m_floorpoints[2].x;
		}

    	glEndList();
    	m_floorSize = _iploc->GetSize();

    	/* Move the camera's center */
    	float buf[3];
    	_camera->GetPosition(buf);
    	_camera->SetCameraLocation(buf[0], buf[1], buf[2], (m_floorpoints[2].x - m_floorpoints[0].x)/2, (m_floorpoints[2].y - m_floorpoints[0].y)/2,  10.0);

    	//printf("resizing floor to: %i\n%i\n", m_floorSize, m_totalFloor);
    }

    //glCallList(m_totalFloor);


/*
        glPushMatrix();
                glBegin(GL_QUADS);
                glColor4f(0.5, 0.5, 1.0, 1.0);
                for(int i = 0; i < 4; i++)
                    glVertex3f(m_floorpoints[i].x, m_floorpoints[i].y, m_floorpoints[i].z);

                glEnd();
            glPopMatrix();
*/

    //glColor4f(0.5, 0.5, 1.0, 1.0);

    /* Vertex array version */
	glEnableClientState( GL_VERTEX_ARRAY);
	glEnableClientState( GL_COLOR_ARRAY);
	glColorPointer(3, GL_UNSIGNED_BYTE, 0, this->m_floorColors);
	glVertexPointer(3, GL_FLOAT, 0, this->m_floorVertices);
	glDrawElements(GL_QUADS, _iploc->GetSize() * 4, GL_UNSIGNED_BYTE, this->m_tileIndices);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	static float recalcCounter = 0.0;
	//if(recalcCounter > 5.0)
	//{
		memset(this->m_floorColors, 0, this->m_numVertices);
		for(int i = 0; i < this->m_numVertices/3; i++)
		{
			m_floorColors[(i*3)+1] = 255;
		}

		recalcCounter = 0.0;
		for(int i = 0; i < _iploc->GetSize(); i++)
		{
#define GREEN_OFFSET 0
			this->m_floorColors[this->m_tileIndices[(i*4)]*3] = (unsigned char)max((int)this->m_floorColors[this->m_tileIndices[i*4]*3], (int)fmin(255.0, _iploc->GetLoad(i)));
			this->m_floorColors[this->m_tileIndices[(i*4)]*3+1] = (unsigned char)min((int)this->m_floorColors[this->m_tileIndices[i*4]*3+1], 255 - (int)fmax(0.0, _iploc->GetLoad(i)-255.0));

			this->m_floorColors[this->m_tileIndices[(i*4)+1]*3] = (unsigned char)max((int)this->m_floorColors[this->m_tileIndices[(i*4)+1]*3], (int)fmin(255.0, _iploc->GetLoad(i)));
			this->m_floorColors[this->m_tileIndices[(i*4)+1]*3+1] = (unsigned char)min((int)this->m_floorColors[this->m_tileIndices[(i*4)+1]*3+1], 255 - (int)fmax(0.0, _iploc->GetLoad(i)-255.0));

			this->m_floorColors[this->m_tileIndices[(i*4)+2]*3] = (unsigned char)max((int)this->m_floorColors[this->m_tileIndices[(i*4)+2]*3], (int)fmin(255.0, _iploc->GetLoad(i)));
			this->m_floorColors[this->m_tileIndices[(i*4)+2]*3+1] = (unsigned char)min((int)this->m_floorColors[this->m_tileIndices[(i*4)+2]*3+1], 255 - (int)fmax(0.0, _iploc->GetLoad(i)-255.0));

			this->m_floorColors[this->m_tileIndices[(i*4)+3]*3] = (unsigned char)max((int)this->m_floorColors[this->m_tileIndices[(i*4)+3]*3], (int)fmin(255.0, _iploc->GetLoad(i)));
			this->m_floorColors[this->m_tileIndices[(i*4)+3]*3+1] = (unsigned char)min((int)this->m_floorColors[this->m_tileIndices[(i*4)+3]*3+1], 255 - (int)fmax(0.0, _iploc->GetLoad(i)-255.0));

			//printf("%i, %lf\n", this->m_floorColors[this->m_tileIndices[i*4]*3], _iploc->GetLoad(i));
			//printf("load = %lf\n", _iploc->GetLoad(i));
		}

	//}
	//else
	//	recalcCounter += getTime();


}

void HadoopDropper::RenderSelected()
{

    /* Round to the nearest multiple of SPACE_BETWEEN */
    //int row = (int)round(m_intercept.intPoint.y/SPACE_BETWEEN);
    //int col = (int)round(m_intercept.intPoint.x/SPACE_BETWEEN);
    //static float halveBetween = (float)SPACE_BETWEEN/2.0;

    //int tmpSelected = row + col*(AREA_SIZE/SPACE_BETWEEN);
    //m_selectedIndex = row + col*(AREA_SIZE/SPACE_BETWEEN);
    //printf("Selected: %i\n", m_selectedIndex);

    if (this->m_cursorOver)
    {
    	char buf[100];
    	sprintf(buf, "%s", this->m_selectHost);
    	_seldisp->SetText(buf);

    }
    else if (this->m_selected && this->m_clickedHost)
    {
    	char buf[100];
    	sprintf(buf, "%s", this->m_clickedHost);
    	_seldisp->SetText(buf);
    }

    //printf("Size: %i\n\n", _iploc->GetSize());
    //if (tmpSelected < _iploc->GetSize())
    //{

    	/* Only execute when the selected block changes */
		//if (m_selectedIndex != tmpSelected)
		//{
			//m_selectedIndex = tmpSelected;
			//printf("ip=%s\n", _iploc->GetIP(m_selectedIndex));

			//_seldisp->SetText(_iploc->GetIP(m_selectedIndex));

			//_seldisp->SetText(_iploc->GetLoad(m_selectedIndex));

		//}
		//point p = _iploc->GetLocation(m_selectedIndex);
        //printf("think you selected %i: %i, %i\n",m_selectedIndex, col*SPACE_BETWEEN, row*SPACE_BETWEEN);
        //printf("actually slected: %lf, %lf\n", p.x, p.y);



    	/* Render the block */
/*
        glPushMatrix();
        glTranslatef(p.x, p.y, 0.01);
        //printf("blah: %i, %i\\n", col * SPACE_BETWEEN, row * SPACE_BETWEEN);
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glBegin(GL_QUADS);
        glVertex3f(-halveBetween, -halveBetween, 0.0);
        glVertex3f(-halveBetween, halveBetween, 0.0);
        glVertex3f(halveBetween, halveBetween, 0.0);
        glVertex3f(halveBetween, -halveBetween, 0.0);
        glEnd();
        glPopMatrix();
        */
    //}
}

