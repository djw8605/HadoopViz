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

	// Render the Visualization Title
	RenderTitle();

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
	static float counter = 1.0;
	static GLuint displaylist = glGenLists(1);
	counter += getTime();
	if (counter >= 0.1)
	{
		counter = 0.0;
		glNewList(displaylist, GL_COMPILE);
		// Render the bar
		glPushMatrix();
		glTranslatef(400.0, 40.0, 0.0);
		glScalef(530.0, 20.0, 1.0);

		glBegin( GL_QUADS);

		glColor4f(0.0, 1.0, 0.0, 1.0);
		glVertex2f(0.0, 0.0);
		glVertex2f(0.0, 1.0);

		glColor4f(1.0, 0.0, 0.0, 1.0);
		glVertex2f(1.0, 1.0);
		glVertex2f(1.0, 0.0);

		glEnd();

		// Put the little tick marks on the bar
		int size = 0;
		const double* loads = _stats->GetHostLoads(&size);
		double maxload = _stats->GetMaxLoad();
		glBegin( GL_LINES);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		double tmpload = 0.0;
		for (int i = 0; i < size; i++)
		{
			tmpload = loads[i] / maxload;
			glVertex3d(tmpload, 0.00, 1.0);
			glVertex3d(tmpload, 1.00, 1.0);

		}
		glEnd();

		glPopMatrix();

		// Label the bar
		glColor4f(0.0, 1.0, 0.0, 1.0);
		freetype::print(*(GetFont()), 0, 10, "Distribution of transfer speeds");

		glColor4f(0.0, 1.0, 0.0, 1.0);
		freetype::print(*(GetFont()), 370, 10, "0.00");

		glColor4f(1.0, 0.0, 0.0, 1.0);
		freetype::print(*(GetFont()), 830, 10, "%.2lf MB/s",
				_stats->GetMaxLoad() / BYTES_PER_MEGABYTE);
		glEndList();
	}
	glCallList(displaylist);


}


void UI::RenderTotalTransfer()
{
	static float update_counter = 1.0;
	static double loadsum = _stats->GetSumLoad() / BYTES_PER_MEGABYTE;
	update_counter += getTime();

	glColor4f(0.0, 1.0, 0.0, 1.0);
	freetype::print(*(GetFont()), 550, 980, "Total Data Transfers: %.2lf MB/s", loadsum);

	if (update_counter > 0.1)
	{
		loadsum = _stats->GetSumLoad() / BYTES_PER_MEGABYTE;
		update_counter = 0.0;
	}

}


void UI::RenderTitle()
{

	glColor4f(1.0, 0.0, 0.0, 1.0);
	freetype::print(*(GetFont()), 10, 980, "Live CMS Tier 1 Transfers");



}





