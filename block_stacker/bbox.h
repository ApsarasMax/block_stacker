#pragma once

#include <vector>
#include <cmath>
#include <cfloat>
#include <typeinfo>
#include "vec.h"
#include "vecOps.h"

class BoundingBox {
protected:	
	bool bEmpty;

public:
	Vec3f center;
	Vec3f vec1;
	Vec3f vec2;
	Vec3f vec3;
	float a1;
	float a2;
	float a3;

public:

	BoundingBox() : bEmpty(true) {}
	BoundingBox(Vec3f center,Vec3f vec1,Vec3f vec2, Vec3f vec3, 
		    float a1, float a2, float a3) : 
		center(center),vec1(vec1),vec2(vec2),vec3(vec3),
		a1(a1),a2(a2),a3(a3),bEmpty(false) {}

	bool isEmpty() { return bEmpty; }

	bool intersect(BoundingBox const& rhs){
	    if(bEmpty) return false;
		
	    Vec3f cross11=VectorOps::crossProduct(this->vec1,rhs.vec1);
	    cross11.normalize();
	    Vec3f cross12=VectorOps::crossProduct(this->vec1,rhs.vec2);
	    cross12.normalize();
	    Vec3f cross13=VectorOps::crossProduct(this->vec1,rhs.vec3);
	    cross13.normalize();
	    Vec3f cross21=VectorOps::crossProduct(this->vec2,rhs.vec1);
	    cross21.normalize();
	    Vec3f cross22=VectorOps::crossProduct(this->vec2,rhs.vec2);
	    cross22.normalize();
	    Vec3f cross23=VectorOps::crossProduct(this->vec2,rhs.vec3);
	    cross23.normalize();
	    Vec3f cross31=VectorOps::crossProduct(this->vec3,rhs.vec1);
	    cross31.normalize();
	    Vec3f cross32=VectorOps::crossProduct(this->vec3,rhs.vec2);
	    cross32.normalize();
	    Vec3f cross33=VectorOps::crossProduct(this->vec3,rhs.vec3);
	    cross33.normalize();

	    return checkIntersect(vec1,rhs) && checkIntersect(vec2,rhs) && 
		   checkIntersect(vec3,rhs) && checkIntersect(rhs.vec1,rhs) &&
		   checkIntersect(rhs.vec2,rhs) && checkIntersect(rhs.vec3,rhs) &&
		   checkIntersect(cross11,rhs) && checkIntersect(cross12,rhs) &&
		   checkIntersect(cross13,rhs) && checkIntersect(cross21,rhs) &&
		   checkIntersect(cross22,rhs) && checkIntersect(cross23,rhs) &&
		   checkIntersect(cross31,rhs) && checkIntersect(cross32,rhs) &&
		   checkIntersect(cross33,rhs);
	}
	
private:
	bool checkIntersect(Vec3f const& L, BoundingBox const& rhs){
	    Vec3f D=rhs.center-this->center;
	    float dotProduct=fabs(D*L);
	    float sum1=fabs(this->a1*this->vec1*L)+fabs(this->a2*this->vec2*L)+fabs(this->a3*this->vec3*L);
	    float sum2=fabs(rhs.a1*rhs.vec1*L)+fabs(rhs.a2*rhs.vec2*L)+fabs(rhs.a3*rhs.vec3*L);
	    //bool returnVal=dotProduct-(sum1+sum2)<FLT_EPSILON;
	    //std::cout<<returnVal<<std::endl;
	    return dotProduct-(sum1+sum2)<FLT_EPSILON;
	}

};
