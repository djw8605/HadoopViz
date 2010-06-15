/*
 * SelectionDisplay.cpp
 *
 *  Created on: Jun 30, 2009
 *      Author: derekweitzel
 */

#include <cstring>
#include "SDL_opengl.h"

#include "SelectionDisplay.h"
#include "display.h"

SelectionDisplay::SelectionDisplay() {
	// TODO Auto-generated constructor stub
	m_text = 0;
}

SelectionDisplay::~SelectionDisplay() {
	// TODO Auto-generated destructor stub
	if(m_text)
		delete m_text;
}


SelectionDisplay* SelectionDisplay::GetInstance()
{

	static SelectionDisplay* inst = new SelectionDisplay();
	return inst;

}

void SelectionDisplay::SetText(char* text)
{

	if(m_text)
		delete m_text;

	m_text = new char[strlen(text)+1];
	strcpy(m_text, text);



}
void SelectionDisplay::ClearText()
{
	if(m_text)
	{
		delete m_text;
		m_text = 0;
	}

}
void SelectionDisplay::Render()
{
	if (!m_text)
		return;

	glMatrixMode( GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluOrtho2D(0.0, 1000.0, 0.0, 1000.0);
	glMatrixMode( GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushMatrix();
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glTranslatef(10.0, 10.0, 0.0);
	freetype::print(*(GetFont()), 10, 10, m_text);
	//printf("writing: %s", m_text);

	glPopMatrix();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}



