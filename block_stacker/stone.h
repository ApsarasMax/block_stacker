// This camera stuff mostly by ehsu, modified by cdanford

#ifndef STONE_H
#define STONE_H

#include "vec.h"
#include "mat.h"
#include "rect.h"
#include "point.h"
#include <vector>

//==========[ class Stone ]===================================================

class Stone
{
private:
    float x;
    float y;
    float z;

    float sLength;
    Vec3f color;

    bool isOnMagnet;
    bool isInPosition;

public:
    Stone(float x1, float y1, float z1, float sLength1, Vec3f color1)
     : x(x1), y(y1), z(z1), sLength(sLength1), color(color1), isOnMagnet(false),
       isInPosition(false) {};
    ~Stone();

    bool inSquare(float x1,float z1){
	float halfLength=sLength/2.0f;
	return x1<x+halfLength && x1>x-halfLength &&
	       z1<z+halfLength && z1>z-halfLength;
    }

    bool getIsOnMagnet(){
	return isOnMagnet;
    }

    float getTopHeight(){
	return y;
    }

    void setIsInPosition(bool isInPosition){
	this->isInPosition=isInPosition;
    }

    bool getIsInPosition(){
	return isInPosition;
    }

    void setIsOnMagnet(bool isOnMagnet){
	this->isOnMagnet=isOnMagnet;
    }

    void setX(float x){
	this->x=x;
    }

    void setY(float y){
	this->y=y;
    }

    void setZ(float z){
	this->z=z;
    }

    float getY(){
	return y;
    }

    Vec3f getPosition(){
	return Vec3f(x,y,z);
    }

    float getLength(){
	return sLength;
    }

    float getBottomHeight(){
	return y-sLength;
    }

    void drawStone(){
    	setAmbientColor( 0.25, 0.25, 0.65 );
    	setDiffuseColor( color[0], color[1], color[2] );

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

