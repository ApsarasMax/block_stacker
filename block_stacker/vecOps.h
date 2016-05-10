#ifndef VECOPS_H
#define VECOPS_H

#include "vec.h"

class VectorOps{
public:
    static Vec3f crossProduct(const Vec3f& a, const Vec3f& b){
	return Vec3f(a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],
	    a[0]*b[1]-a[1]*b[0]);	
    }

    static Vec3f elementMultiplication(const Vec3f& a, const Vec3f& b){
	float element1=a[0]*b[0];
	element1= element1!=element1 ? 0.0 : element1;
	float element2=a[1]*b[1];
	element2= element2!=element2 ? 0.0 : element2;
	float element3=a[2]*b[2];
	element3= element3!=element3 ? 0.0 : element3;
	return Vec3f(element1,element2,element3);
    }
};

#endif
