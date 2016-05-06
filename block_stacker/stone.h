// This camera stuff mostly by ehsu, modified by cdanford

#ifndef STONE_H
#define STONE_H

#include "vec.h"
#include "mat.h"
#include "rect.h"
#include "point.h"
#include <vector>

//==========[ class Stone ]===================================================


class stone
{
public:
    stone(float x, float y, float z, float sLength, Vec3f color);
    stone(float x, float y, float z, float sLength);
    ~stone();

    float x;
    float y;
    float z;

    float sLength;
    Vec3f color;

    bool isHooked;

    void drawStone(){

        glBegin( GL_QUADS );

        glNormal3d( 1.0 ,0.0, 0.0);         // +x side
        glVertex3d( x + sLength / 2.0, y - sLength, z + sLength / 2.0);
        glVertex3d( x + sLength / 2.0, y - sLength, z - sLength / 2.0);
        glVertex3d( x + sLength / 2.0,  y, z - sLength / 2.0);
        glVertex3d( x + sLength / 2.0,  y, z + sLength / 2.0);

        glNormal3d( 0.0 ,0.0, -1.0);        // -z side
        glVertex3d( x + sLength / 2.0, y - sLength, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0, y - sLength, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y, z - sLength / 2.0);
        glVertex3d( x + sLength / 2.0,  y, z - sLength / 2.0);

        glNormal3d(-1.0, 0.0, 0.0);         // -x side
        glVertex3d( x - sLength / 2.0, y - sLength, z + sLength / 2.0);
        glVertex3d( x - sLength / 2.0, y - sLength, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y, z + sLength / 2.0);

        glNormal3d( 0.0, 0.0, 1.0);         // +z side
        glVertex3d( x + sLength / 2.0, y - sLength, z + sLength / 2.0);
        glVertex3d( x - sLength / 2.0, y - sLength, z + sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y, z + sLength / 2.0);
        glVertex3d( x + sLength / 2.0,  y, z + sLength / 2.0);

        glNormal3d( 0.0, 1.0, 0.0);         // top (+y)
        glVertex3d( x + sLength / 2.0,  y, z + sLength / 2.0);
        glVertex3d( x + sLength / 2.0,  y, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y, z + sLength / 2.0);

        glNormal3d( 0.0,-1.0, 0.0);         // bottom (-y)
        glVertex3d( x + sLength / 2.0,  y - sLength, z + sLength / 2.0);
        glVertex3d( x + sLength / 2.0,  y - sLength, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y - sLength, z - sLength / 2.0);
        glVertex3d( x - sLength / 2.0,  y - sLength, z + sLength / 2.0);

        glEnd();

    }
};

#endif

