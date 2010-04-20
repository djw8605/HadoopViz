

//#include <GL/glut.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "display.h"
#include "events.h"
#include "init.h"
#include "config.h"
#include <unistd.h>
#include "Camera/Camera.h"
#include <getopt.h>
#include <vector>

using namespace std;

char* serverHost;
short serverPort;
char* dataDir;

vector<char*> modified_ips;

void usage()
{
    printf(
            "Usage: hadoopviz <options>\n"
            "\t-s, --server SERVER\tOverride the default server (%s)\n"
            "\t-p, --port PORT\t\tOverride the default server port (%i)\n"
            "\t-a, --datadir DATADIR\tThe datadir with textures/ and fonts/ directory\n"
            "\n", DEFAULT_SERVER, DEFAULT_PORT);


}



int main(int argc, char *argv[])
{
        //int windowWidth = 1024;
        //int windowHeight = 768;
        serverHost = new char[strlen(DEFAULT_SERVER)+1];
        strcpy(serverHost, DEFAULT_SERVER);
        serverHost[strlen(DEFAULT_SERVER)] = '\0';
        serverPort = DEFAULT_PORT;

        dataDir = new char[strlen(DEFAULT_DATADIR)+1];
        strcpy(dataDir, DEFAULT_DATADIR);
        dataDir[strlen(DEFAULT_DATADIR)] = '\0';



	/* Process Arguments */

	//int option_char;

	const char* short_options = "hs:p:t:";
	static struct option long_options[] =
	    {
	            {"help", no_argument, 0, 'h'},
	            {"server", required_argument, 0, 's'},
	            {"port", required_argument, 0, 'p'},
	            {"datadir", required_argument, 0, 'a'},
	            {"modified", required_argument, 0, 'm'},
	            {0, 0, 0, 0}
	    };

	int c;
	short tmpValue;
	char* tmp;
	while(1)
	{
	    int option_index = 0;
	    c = getopt_long(argc, argv, short_options, long_options, &option_index);

	    /* End of options?? */
	    if(c == -1)
	        break;

	    switch(c)
	    {
	    case 0:
	        /* Flag option, don't currently have any */
	        if(long_options[option_index].flag != 0)
	            break;

	        break;

	    case 'h':
	        usage();
	        exit(0);

	    case 's':
	        if(serverHost)
	            delete serverHost;
	        serverHost = new char[strlen(optarg)+1];
	        strcpy(serverHost, optarg);
	        serverHost[strlen(optarg)] = '\0';
	        break;

	    case 'p':
	        tmpValue = (short)atoi(optarg);
	        if(tmpValue != 0)
	        {
	            serverPort = tmpValue;
	        }
	        else
	        {
	            printf("Port invalid: %s\n", optarg);
	        }

	    case 'a':
	    	if(dataDir)
	    		delete dataDir;
	    	dataDir = new char[strlen(optarg)+1];
	    	strcpy(dataDir, optarg);
	    	dataDir[strlen(optarg)] = '\0';
	    	break;

	    case 'm':
	    	tmp = new char[strlen(optarg)+1];
	    	strcpy(tmp, optarg);
	    	tmp[strlen(optarg)] = '\0';
	    	modified_ips.push_back(tmp);
	    	break;

	    case '?':
	        break;

	    default:
	        usage();
	        exit(-1);
	        break;


	    }



	}
	//for (int i = 0; i < modified_ips.size(); i++)
	//	printf("%s\n", modified_ips[i]);

	/* Setup Window */
	//glutInit(&argc, argv);
	if ( SDL_Init(SDL_INIT_VIDEO) != 0) {
	    printf("Unable to initialize SDL: %s\n", SDL_GetError());
	    return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_Surface* screen = SDL_SetVideoMode(640, 480, 0, SDL_OPENGL | SDL_RESIZABLE);
	SDL_SetVideoMode(640, 480, 0, SDL_OPENGL | SDL_RESIZABLE);
	SDL_WM_SetCaption("SecViz","SecViz");
    init();

    SDL_Event event;
    int running = 1;
    SDL_KeyboardEvent kb;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                kb = event.key;
                if(kb.keysym.sym == SDLK_UP)
                    _camera->ModifyRotationRadius(-1.0);
                else if (kb.keysym.sym == SDLK_DOWN)
                    _camera->ModifyRotationRadius(1.0);
                break;
            case SDL_KEYUP:
                //DisplayState(&event.key);
                //DisplayModifiers(&event.key);
                //DisplayKey(&event.key);
                break;
            case SDL_QUIT:
                running = 0;
                break;

            case SDL_VIDEORESIZE:


                reshape(event.resize.w, event.resize.h);

                break;

            case SDL_MOUSEMOTION:
                mouseMove(event.motion.x, event.motion.y);

                break;

            case SDL_MOUSEBUTTONDOWN:
            	mouseClick(event.button.x, event.button.y);

            }
        }
        display();
    }


    //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //glutInitWindowSize(windowWidth, windowHeight);
	//glutInitWindowPosition(0,0);
	//glutCreateWindow("Hadoop Viz");
	//glutFullScreen();
	//glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	//glutKeyboardFunc(keyboard);
	//glutSpecialFunc(specialKeyboard);
	//glutIdleFunc(display);

	/* Intialize everything */
	//init();



	/* Enter Game Loop */

	//glutMainLoop();

	ShutDown();

	return 0;


}

