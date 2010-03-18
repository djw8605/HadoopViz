/*
 * Drops.h
 *
 *  Created on: Jun 16, 2009
 *      Author: dweitzel
 */

#ifndef DROPS_H_
#define DROPS_H_

#include <deque>
#ifdef USE_VBO
#include "ModelLoader/CModel3DS.h"
#endif
#include "Math/Math.h"
#include <cstdio>
#include "SDL_opengl.h"

#include "HadoopDropper/SingleDrop.h"
#include "HadoopDropper/DropListener.h"




using namespace std;

class Drops
{
public:
    Drops();
    virtual ~Drops();
    void Render();
    void InitDisplayLists();


private:

    deque<SingleDrop*> m_drops;

    void RenderDrop(SingleDrop* s );

    GLuint m_dropList;
    GLuint m_sphereList;

    DropListener* m_dl;

#ifdef USE_VBO
    CModel3DS* m_raindrop;
#endif

    int m_VBOSupported;


};



#endif /* DROPS_H_ */
