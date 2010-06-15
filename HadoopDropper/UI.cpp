/*
 * UI.cpp
 *
 *  Created on: Jun 14, 2010
 *      Author: derekweitzel
 */

#include "UI.h"
#include "display.h"
#include "GlobalStats.h"
#include "defaults.h"

UI::UI()
{
	// TODO Auto-generated constructor stub

}

UI::~UI()
{
	// TODO Auto-generated destructor stub
}


UI* UI::GetInstance()
{
	static UI* inst = new UI();
	return inst;

}


void UI::Render()
{

	// Setup 2d Environment
	Initialize2d();

	// Render the color index bar
	RenderColorBar();

	// Render the total transfer bar
	RenderTotalTransfer();

	// Shutdown 2d Environment
	Deinitialize2d();

}


void UI::Initialize2d()
{

	glMatrixMode( GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluOrtho2D(0.0, 1000.0, 0.0, 1000.0);
	glMatrixMode( GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


}


void UI::Deinitialize2d()
{

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);


}


void UI::RenderColorBar()
{

	// Render the bar
	glPushMatrix();
	glTranslatef(500.0, 40.0, 0.0);
	glScalef(450.0, 20.0, 0.0);

	glBegin(GL_QUADS);

	glColor4f(0.0, 1.0, 0.0, 1.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.0, 1.0);

	glColor4f(1.0, 0.0, 0.0, 1.0);
	glVertex2f(1.0, 1.0);
	glVertex2f(1.0, 0.0);

	glEnd();
	glPopMatrix();

	// Label the bar
	glColor4f(0.0, 1.0, 0.0, 1.0);
	freetype::print(*(GetFont()), 470, 10, "0.00");

	glColor4f(1.0, 0.0, 0.0, 1.0);
	freetype::print(*(GetFont()), 870, 10, "%.2lf MB/s", _stats->GetMaxLoad()/BYTES_PER_MEGABYTE);


}


void UI::RenderTotalTransfer()
{
	static float update_counter = 1.0;
	static double loadsum = _stats->GetSumLoad()/BYTES_PER_MEGABYTE;
	update_counter += getTime();

		glColor4f(0.0, 1.0, 0.0, 1.0);
		freetype::print(*(GetFont()), 10, 980, "Total Transfers: %.2lf MB/s", loadsum);

	if (update_counter > 0.1)
	{
		loadsum = _stats->GetSumLoad()/BYTES_PER_MEGABYTE;
		update_counter = 0.0;
	}

}




