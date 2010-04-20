/*
 * HadoopDropper.h
 *
 *  Created on: Jun 16, 2009
 *      Author: dweitzel
 */

#ifndef HADOOPDROPPER_H_
#define HADOOPDROPPER_H_

#include "AbstractScene/AbstractScene.h"
#include "HadoopDropper/Drops.h"
#include "Math/Math.h"

class HadoopDropper: public AbstractScene
{
public:
    HadoopDropper();
    virtual ~HadoopDropper();
    virtual void Render();                 /* Render the scene */
            virtual void Init();                   /* Initialize the scene */
            virtual void DeInit();                 /* DeInitialize the scene */
            virtual void Select(int x, int y);     /* Select a portion of the screen, such as a mouse click */
            virtual void MouseMove(int x, int y);
            virtual void Reshape(int w, int h);
            virtual void Reset();

private:
    void RenderFloor();
    void RenderNodes();
    void RenderSelected();
    void InitDisplayLists();
    point * m_selectedPoint;
    bool m_selected;
    bool m_checkselect;
    char* m_clickedHost;
    char* m_selectHost;
    int mouse[2];
    char* m_selectIP;
    Drops* drops;
    point m_floorpoints[4];
    bool m_cursorOver;
    distloc m_intercept;
    int m_selectedIndex;
    unsigned int m_floorList;
    bool m_supportLists;
    unsigned int m_floorTile;
    int m_floorSize;
    unsigned int m_totalFloor;
    GLfloat* m_floorVertices;
    int m_numVertices;
    int m_numTiles;
    GLubyte* m_tileIndices;
    GLubyte* m_floorColors;
};

#endif /* HADOOPDROPPER_H_ */
