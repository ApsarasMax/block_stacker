// This camera stuff mostly by ehsu, modified by cdanford

#ifndef STONE_H
#define STONE_H

#include "vec.h"
#include "mat.h"
#include "rect.h"
#include "point.h"
#include "bbox.h"
#include <vector>

#include <ode/ode.h>

#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/gl.h>  // Header File For The OpenGL32 Library
#include <GL/glu.h> // Header File For The GLu32 Library

//==========[ class Stone ]===================================================
GLuint texid;
GLuint texwidth=2;
GLuint texheight=2;

GLubyte texData[64] =
{
  0xbF,0xbF,0xFF,0xFF, // blue // comments are not corresponding
  0xa0,0xFF,0xFF,0xFF, // rgreen
  0x00,0xFF,0xFF,0xFF, // rgreen
  0xFF,0xFF,0xFF,0xFF, // blue

  0x00,0xFF,0xFF,0xFF, // rgreen
  0xbF,0xbF,0xFF,0xFF, // blue
  0xFF,0xFF,0xFF,0xFF, // blue
  0xa0,0xFF,0xFF,0xFF, // rgreen
  0xa0,0xFF,0xFF,0xFF, // rgreen
  0xFF,0xFF,0xFF,0xFF, // blue
  0xbF,0xbF,0xFF,0xFF, // blue
  0x00,0xFF,0xFF,0xFF, // rgreen

  0xFF,0xFF,0xFF,0xFF, // blue
  0x00,0xFF,0xFF,0xFF, // rgreen
  0xa0,0xFF,0xFF,0xFF, // rgreen
  0xbF,0xbF,0xFF,0xFF, // blue
};


class Stone : public BoundingBox
{
private:
    const float density=0.71e-3; // kg/m3

    float x;
    float y;
    float z;

    Vec3f color;

    bool isOnMagnet;

public:
    float sLength;
    bool isInPosition;
    float inPositionY;

    //TODO: add rigid body motion

    
public:
    dBodyID body;
    dGeomID geom;
    dMass mass;

    Stone(float x1, float y1, float z1, float sLength1, Vec3f color1)
     : x(x1), y(y1), z(z1), sLength(sLength1), color(color1), isOnMagnet(false),
       isInPosition(false) {
	bEmpty=false;
	center=Vec3f(x1,y1-sLength1/2.0f,z1);
	vec1=Vec3f(1,0,0);
	a1=sLength1/2.0f;
	vec2=Vec3f(0,1,0);
	a2=sLength1/2.0f;
	vec3=Vec3f(0,0,1);
	a3=sLength1/2.0f;

	dMassSetBox(&mass,density,sLength,sLength,sLength);
    };
    
    ~Stone();
        

    bool inStone(float x1,float z1){
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
	center=Vec3f(x,y-sLength/2.0f,z);
	this->x=x;
    }

    void setY(float y){
	center=Vec3f(x,y-sLength/2.0f,z);
	this->y=y;
    }

    void setZ(float z){
	center=Vec3f(x,y-sLength/2.0f,z);
	this->z=z;
    }

    float getX(){
	return x;
    }

    float getY(){
	return y;
    }

    float getZ(){
	return z;
    }

    Vec3f getPosition(){
	return Vec3f(x,y,z);
    }

    Vec3f getCenter(){
	return center;
    }

    std::vector<Vec3f*> getBottomPoints(){
	std::vector<Vec3f*> returnVal;
	returnVal.push_back(new Vec3f(x-sLength/2.0f,y-sLength,z-sLength/2.0f));
	returnVal.push_back(new Vec3f(x-sLength/2.0f,y-sLength,z+sLength/2.0f));
	returnVal.push_back(new Vec3f(x+sLength/2.0f,y-sLength,z-sLength/2.0f));
	returnVal.push_back(new Vec3f(x+sLength/2.0f,y-sLength,z+sLength/2.0f));
	return returnVal;
    }

    float getSLength(){
	return sLength;
    }

    float getBottomHeight(){
	return y-sLength;
    }

    void updatePosition(){
	const dReal* position=dBodyGetPosition(body);
	//std::cout<<position[0]<<","<<position[1]<<","<<position[2]<<std::endl;
	center=Vec3f(position[0],position[1],position[2]);
    }

    void drawStone(){
    	setAmbientColor( 0.25, 0.25, 0.65 );
        

    if(!isInPosition){//!isInPosition
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

    }else{

	const dReal* rotation=dBodyGetRotation(body);
	Mat3f rotation_matrix(rotation[0],rotation[1],rotation[2],
			      rotation[4],rotation[5],rotation[6],
			      rotation[8],rotation[9],rotation[10]);

        setDiffuseColor( 1, 1, 1 );

        glGenTextures(1,&texid);
        glBindTexture(GL_TEXTURE_2D,texid);  //Sélectionne ce n°
        glTexImage2D (GL_TEXTURE_2D, 0, 3, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST

        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);      // Enables Smooth Color Shading

        glBegin( GL_QUADS );

	    {
		Vec3f normal=rotation_matrix*Vec3f(1,0,0);
	        normal.normalize();
                glNormal3d( normal[0] ,normal[1], normal[2]);         // +x side
		Vec3f vertex1=center+rotation_matrix*Vec3f(sLength/2.0,-sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 0.0f); 
		glVertex3d( vertex1[0], vertex1[1], vertex1[2]);
		Vec3f vertex2=center+rotation_matrix*Vec3f(sLength/2.0,-sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 0.0f); 
		glVertex3d( vertex2[0], vertex2[1], vertex2[2]);
		Vec3f vertex3=center+rotation_matrix*Vec3f(sLength/2.0,sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 1.0f); 
		glVertex3d( vertex3[0],  vertex3[1], vertex3[2]);
		Vec3f vertex4=center+rotation_matrix*Vec3f(sLength/2.0,sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 1.0f); 
		glVertex3d( vertex4[0],  vertex4[1], vertex4[2]);
	    }

	    {
		Vec3f normal=rotation_matrix*Vec3f(0,0,-1);
		normal.normalize();
                glNormal3d( normal[0] ,normal[1], normal[2]);        // -z side
		Vec3f vertex1=center+rotation_matrix*Vec3f(sLength/2.0,-sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 0.0f); 
		glVertex3d( vertex1[0], vertex1[1], vertex1[2]);
		Vec3f vertex2=center+rotation_matrix*Vec3f(-sLength/2.0,-sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 1.0f); 
		glVertex3d( vertex2[0], vertex2[1], vertex2[2]);
		Vec3f vertex3=center+rotation_matrix*Vec3f(-sLength/2.0,sLength/2.0,-sLength/2.0);
                glTexCoord2f(0.0f, 1.0f); 
		glVertex3d( vertex3[0],  vertex3[1], vertex3[2]);
		Vec3f vertex4=center+rotation_matrix*Vec3f(sLength/2.0,sLength/2.0,-sLength/2.0);
                glTexCoord2f(0.0f, 0.0f); 
		glVertex3d( vertex4[0], vertex4[1], vertex4[2]);
	    }

	    {
		Vec3f normal=rotation_matrix*Vec3f(-1,0,0);
		normal.normalize();
                glNormal3d(normal[0], normal[1], normal[2]);         // -x side
		Vec3f vertex1=center+rotation_matrix*Vec3f(-sLength/2.0,-sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 1.0f); 
		glVertex3d( vertex1[0], vertex1[1], vertex1[2]);
		Vec3f vertex2=center+rotation_matrix*Vec3f(-sLength/2.0,-sLength/2.0,-sLength/2.0);
                glTexCoord2f(0.0f, 0.0f); 
		glVertex3d( vertex2[0], vertex2[1], vertex2[2]);
		Vec3f vertex3=center+rotation_matrix*Vec3f(-sLength/2.0,sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 0.0f); 
		glVertex3d( vertex3[0],  vertex3[1], vertex3[2]);
		Vec3f vertex4=center+rotation_matrix*Vec3f(-sLength/2.0,sLength/2.0,sLength/2.0);
                glTexCoord2f(1.0f, 1.0f); 
		glVertex3d( vertex4[0],  vertex4[1], vertex4[2]);
	    }

	    {
		Vec3f normal=rotation_matrix*Vec3f(0,0,1);
	  	normal.normalize();
                glNormal3d( normal[0], normal[1], normal[2]);         // +z side
		Vec3f vertex1=center+rotation_matrix*Vec3f(sLength/2.0,-sLength/2.0,sLength/2.0);
                glTexCoord2f(1.0f, 1.0f); 
		glVertex3d( vertex1[0], vertex1[1], vertex1[2]);
		Vec3f vertex2=center+rotation_matrix*Vec3f(-sLength/2.0,-sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 1.0f); 
		glVertex3d( vertex2[0], vertex2[1], vertex2[2]);
		Vec3f vertex3=center+rotation_matrix*Vec3f(-sLength/2.0,sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 0.0f); 
		glVertex3d( vertex3[0],  vertex3[1], vertex3[2]);
		Vec3f vertex4=center+rotation_matrix*Vec3f(sLength/2.0,sLength/2.0,sLength/2.0);
                glTexCoord2f(1.0f, 0.0f); 
		glVertex3d( vertex4[0],  vertex4[1], vertex4[2]);
	    }

	    {
	 	Vec3f normal=rotation_matrix*Vec3f(0,1,0);
		normal.normalize();
                glNormal3d( normal[0], normal[1], normal[2]);         // top (+y)
		Vec3f vertex1=center+rotation_matrix*Vec3f(sLength/2.0,sLength/2.0,sLength/2.0);
                glTexCoord2f(1.0f, 0.0f); 
		glVertex3d( vertex1[0],  vertex1[1], vertex1[2]);
		Vec3f vertex2=center+rotation_matrix*Vec3f(sLength/2.0,sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 1.0f); 
		glVertex3d( vertex2[0],  vertex2[1], vertex2[2]);
		Vec3f vertex3=center+rotation_matrix*Vec3f(-sLength/2.0,sLength/2.0,-sLength/2.0);
                glTexCoord2f(0.0f, 1.0f); 
		glVertex3d( vertex3[0],  vertex3[1], vertex3[2]);
		Vec3f vertex4=center+rotation_matrix*Vec3f(-sLength/2.0,sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 0.0f); 
		glVertex3d( vertex4[0],  vertex4[1], vertex4[2]);
	    }

	    {
		Vec3f normal=rotation_matrix*Vec3f(0,-1,0);
		normal.normalize();
                glNormal3d( normal[0],normal[1], normal[2]);         // bottom (-y)
		Vec3f vertex1=center+rotation_matrix*Vec3f(sLength/2.0,-sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 0.0f); 
		glVertex3d( vertex1[0],  vertex1[1], vertex1[2]);
		Vec3f vertex2=center+rotation_matrix*Vec3f(sLength/2.0,-sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 0.0f); 
		glVertex3d( vertex2[0],  vertex2[1], vertex2[2]);
		Vec3f vertex3=center+rotation_matrix*Vec3f(-sLength/2.0,-sLength/2.0,-sLength/2.0);
                glTexCoord2f(1.0f, 1.0f); 
		glVertex3d( vertex3[0],  vertex3[1], vertex3[2]);
		Vec3f vertex4=center+rotation_matrix*Vec3f(-sLength/2.0,-sLength/2.0,sLength/2.0);
                glTexCoord2f(0.0f, 1.0f); 
		glVertex3d( vertex4[0],  vertex4[1], vertex4[2]);
	    }

        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    }

};


#endif

