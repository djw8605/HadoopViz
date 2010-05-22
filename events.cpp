
/*
 * This file includes anything and everything to do with processing events.
 *
 *
 *
 *
 */

#include "defaults.h"
#include "SDL.h"
#include "SDL_opengl.h"
//#include <GL/glut.h>
#include <stdio.h>
#include "display.h"

void reshape(int w, int h)
{
    SDL_SetVideoMode(w, h, 0, SDL_OPENGL | SDL_RESIZABLE);
    DisplayReshape();
    GetDisplayed()->Reshape(w, h);
	/*
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, (GLfloat) w/ (GLfloat) h, 40, 5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/


    //printf("Size = %i, %i\n", w, h);


}

/* Old GLUT function */
void mouse(int button, int state, int x, int y)
{

    //int width = SDL_GetVideoSurface()->w; //glutGet(GLUT_WINDOW_WIDTH);
    //int height = SDL_GetVideoSurface()->h; //glutGet(GLUT_WINDOW_HEIGHT);

    //printf("Width = %i, H = %i\n", width, height);



    //int cursorX = ((float)x/(float)width)*1024.0;
    //int cursorY = ((float)y/(float)height)*768.0;

    //printf("Inside mouse: (%i, %i)\n", cursorX, cursorY);
	//_cursor->ProcessCoordinates(cursorX, cursorY, cursorX, cursorY);

	//int points[2];
	//_cursor->GetCurrentCursor(points);

	//GetDisplayed()->Select(points[0], points[1]);

}

void mouseMove(int x, int y)
{
    GetDisplayed()->MouseMove(x, y);

}


void keyboard(unsigned char key, int x, int y)
{

	if (key == 'r')
		GetDisplayed()->Reset();


}



void specialKeyboard(int key, int x, int y)
{


}



