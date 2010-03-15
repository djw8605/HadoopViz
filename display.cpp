//#include <GL/glut.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include <sys/time.h>
#include <unistd.h>

#include <assert.h>

#include <iostream>
#include <sstream>


#include "display.h"
#include "Camera/Camera.h"
#include "AbstractScene/AbstractScene.h"
#include "FreeType/FreeType.h"
#include "config.h"
//#include "TextPrinter/BitMapText.h"

#ifndef NO_AUDIO
#include "Audio/Audio.h"
#endif

#include "HadoopDropper/HadoopDropper.h"

extern char* dataDir;

struct SceneNode {
    AbstractScene* scene;
    SceneNode* next;
};

SceneNode* scenes = NULL;
SceneNode* currentScene = NULL;

/* Definition of function below */
void updateTime();

void InitDisplay()
{
    updateTime();


    /* Add all of the scenes here */
    AddScene(new HadoopDropper());

    /* Internal Intializers */
    currentScene = scenes;
    currentScene->scene->Init();


}


void displayScene()
{

    currentScene->scene->Render();

}


void display()
{

    /* Update the time for movement, and the like */
    updateTime();

    /* Tell the camera to move to next frame (for animations and such) */
    _camera->NextFrame();

    static float frameCounter = 0.0;
    frameCounter += getTime();
    static int intFrame = 0;
    intFrame++;
    if(frameCounter > 1.0)
    {
    	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bfps:%i", intFrame);
    	fflush(stdout);
    	intFrame = 0;
    	frameCounter = 0;
    }


    /* Generic OpenGL stuff */
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();

     /* Move Camera */
    /*camera->positionCamera(); */


    /* List everything to display to the screen here */
    displayScene();


    /* And now the normal OpenGL Cleanup stuff */
    glPopMatrix();
    //glutSwapBuffers();
    SDL_GL_SwapBuffers();

    //usleep(10000);

}


void AddScene(AbstractScene* toAdd)
{
    /* Add a scene to the list */
    SceneNode* tmpNode = scenes;
    if (tmpNode == NULL)
    {
        scenes = new SceneNode;
        scenes->scene = toAdd;
        scenes->next = NULL;

    }
    else
    {
        while (tmpNode != NULL)
        {
            tmpNode = tmpNode->next;
        }
        tmpNode->next = new SceneNode;
        tmpNode->next->scene = toAdd;
        tmpNode->next->next = NULL;
    }




}


void NextScene()
{
	assert(scenes);
	assert(currentScene);

	/* Tell the scene that it is done */
	currentScene->scene->DeInit();

	currentScene = currentScene->next;

	/* Tell the new scene to get ready */
	currentScene->scene->Init();





}


void PrevScene()
{

	assert(scenes);

	//SceneNode* tmpScenes = scenes;




}



AbstractScene* GetDisplayed()
{
    return currentScene->scene;


}

freetype::font_data* font = NULL;
freetype::font_data* GetFont()
{

    if (font == NULL)
    {
        try
        {
            font = new freetype::font_data;
            font->init("FreeType/Test.ttf", 16);
        }
        catch (std::exception &e)
        {
            printf ("%s\n", e.what());
        }

    }

    return font;



}

double storeTime = 0, lastTime = 0;
double getTime() {
    return storeTime - lastTime;
}

void updateTime() {

    lastTime = storeTime;
    timeval t;
    gettimeofday(&t, 0);
    //store seconds since epoch
    storeTime = t.tv_sec;
    //store micoseconds of that second, so divide by million
    storeTime += ((double)t.tv_usec / 1000000);

}


void ShutDown()
{
    printf("Shutting down\n");
    fflush(stdout);
    /* delete the scenes */
    delete GetDisplayed();

	if(font)
	{
		font->clean();
		delete font;
	}
#ifndef NO_AUDIO
    delete _audio;
#endif


    //KillFont();



}


void DisplayReshape() {
	if (font) {
		font->clean();
		delete font;
		try {
			font = new freetype::font_data;
			stringstream ss;
			ss << dataDir << "/fonts/Test.ttf";
			font->init(ss.str().c_str(), 16);
		} catch (std::exception &e) {
			printf("%s\n", e.what());
		}
	}


}


int IsExtensionSupported( char* szTargetExtension )
{
        const unsigned char *pszExtensions = NULL;
        const unsigned char *pszStart;
        unsigned char *pszWhere, *pszTerminator;

        // Extension names should not have spaces
        pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
        if( pszWhere || *szTargetExtension == '\0' )
                return false;

        // Get Extensions String
        pszExtensions = glGetString( GL_EXTENSIONS );

        // Search The Extensions String For An Exact Copy
        pszStart = pszExtensions;
        for(;;)
        {
                pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
                if( !pszWhere )
                        break;
                pszTerminator = pszWhere + strlen( szTargetExtension );
                if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
                        if( *pszTerminator == ' ' || *pszTerminator == '\0' )
                                return true;
                pszStart = pszTerminator;
        }
        return false;
}



