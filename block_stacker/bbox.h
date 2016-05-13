#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include <cfloat>
#include <typeinfo>
#include <assert.h>
#include "vec.h"
#include "vecOps.h"

using std::cout;
using std::endl;

class BoundingBox;

class Plane{
//ax+by+cz=d
public:
    float a;
    float b;
    float c;
    float d;

    Plane(float a,float b,float c, float d) : a(a),b(b),c(c),d(d) {}

    Plane(Vec3f norm,Vec3f p){
	a=norm[0];
	b=norm[1];
	c=norm[2];
	d=norm*p;
    }

    Plane(Vec3f p1,Vec3f p2,Vec3f p3){
	Vec3f edge1=p2-p1;
	Vec3f edge2=p3-p1;
	Vec3f norm=VectorOps::crossProduct(edge1,edge2);
	norm.normalize();
	a=norm[0];
	b=norm[1];
	c=norm[2];
	d=norm*p1;
    }

    float distance(Vec3f const& p){
	return abs(a*p[0]+b*p[1]+c*p[2]-d)/sqrt(pow(a,2)+pow(b,2)+pow(c,2));
    }

    Vec3f projectionPt(Vec3f const& p){
	Vec3f targetPt(0,0,d/c);
	Vec3f norm(a,b,c);
	return p+norm*(targetPt-p)*norm;
    }

};

/*
class Contact{
public:
    BoundingBox* a;
    BoundingBox* b;
    Vec3f p;
    Vec3f n;
    Vec3f ea;
    Vec3f eb;

    bool vf;

    Contact(void){}

    Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n) : 
	a(a),b(b),p(p),n(n),vf(true) {}

    Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
	a(a),b(b),p(p),n(n),ea(ea),eb(eb),vf(false) {}
};
*/

class BoundingBox {
public:	
	bool bEmpty;
	std::vector<Plane*> planes;
	std::vector<Vec3f*> vertices; 

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
		a1(a1),a2(a2),a3(a3),bEmpty(false) {
	    updatePlanes();
	    updateVertices();
	}

	//friend std::vector<Contact*> getContancts(BoundingBox* A, BoundingBox* B);

	void updateVertices(){
	    vertices.clear();
	    vertices.push_back(new Vec3f{center+a1*vec1+a2*vec2+a3*vec3});
	    vertices.push_back(new Vec3f{center+a1*vec1+a2*vec2-a3*vec3});
	    vertices.push_back(new Vec3f{center+a1*vec1-a2*vec2+a3*vec3});
	    vertices.push_back(new Vec3f{center+a1*vec1-a2*vec2-a3*vec3});
	    vertices.push_back(new Vec3f{center-a1*vec1+a2*vec2+a3*vec3});
	    vertices.push_back(new Vec3f{center-a1*vec1+a2*vec2-a3*vec3});
	    vertices.push_back(new Vec3f{center-a1*vec1-a2*vec2+a3*vec3});
	    vertices.push_back(new Vec3f{center-a1*vec1-a2*vec2-a3*vec3});
	}

	void updatePlanes(){
	    planes.clear();
	    planes.push_back(new Plane(vec1,center+a1*vec1));
	    planes.push_back(new Plane(vec2,center+a2*vec2));
	    planes.push_back(new Plane(vec3,center+a3*vec3));
	    planes.push_back(new Plane(-vec1,center-a1*vec1));
	    planes.push_back(new Plane(-vec2,center-a2*vec2));
	    planes.push_back(new Plane(-vec3,center-a3*vec3));
	}

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

	    bool returnVal=checkIntersect(vec1,rhs) && checkIntersect(vec2,rhs) && 
		   checkIntersect(vec3,rhs) && checkIntersect(rhs.vec1,rhs) &&
		   checkIntersect(rhs.vec2,rhs) && checkIntersect(rhs.vec3,rhs) &&
		   checkIntersect(cross11,rhs) && checkIntersect(cross12,rhs) &&
		   checkIntersect(cross13,rhs) && checkIntersect(cross21,rhs) &&
		   checkIntersect(cross22,rhs) && checkIntersect(cross23,rhs) &&
		   checkIntersect(cross31,rhs) && checkIntersect(cross32,rhs) &&
		   checkIntersect(cross33,rhs);

	    return returnVal; 
	}
	
public:
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
/*
bool insideSquare(Vec3f p,Vec3f vertex1,Vec3f vertex2,Vec3f vertex3,Vec3f vertex4){
    float length=(vertex1-vertex2).length();
    float d1=(VectorOps::crossProduct(p-vertex1,p-vertex2)).length()/(vertex2-vertex1).length();
    float d2=(VectorOps::crossProduct(p-vertex2,p-vertex3)).length()/(vertex3-vertex2).length();
    float d3=(VectorOps::crossProduct(p-vertex3,p-vertex4)).length()/(vertex4-vertex3).length();
    float d4=(VectorOps::crossProduct(p-vertex4,p-vertex1)).length()/(vertex1-vertex4).length();
    return d1<length && d2<length && d3<length && d4<length;
}

int negativeFlag(float x){
    return x<0;
}

std::vector<Contact*> getContacts(BoundingBox* A, BoundingBox* B){
    float tolerence=0.05;
    std::vector<Contact*> returnVal;

    //vertex-face

    std::vector<Vec3f*> A_face_normals;
    std::vector<Vec3f*> B_face_normals;

    if(!A->checkIntersect(A->vec1,*B)){
	A_face_normals.push_back(&(A->vec1));
    }

    if(!A->checkIntersect(A->vec2,*B)){
	A_face_normals.push_back(&(A->vec2));
    }

    if(!A->checkIntersect(A->vec3,*B)){
	A_face_normals.push_back(&(A->vec3));
    }

    assert(A_face_normals.size()<=1);

    if(!A->checkIntersect(B->vec1,*B)){
	B_face_normals.push_back(&(B->vec1));
    }

    if(!A->checkIntersect(B->vec2,*B)){
	B_face_normals.push_back(&(B->vec2));
    }

    if(!A->checkIntersect(A->vec3,*B)){
	B_face_normals.push_back(&(B->vec3));
    }

    assert(B_face_normals.size()<=1);

    if(A_face_normals.size()>0 && B_face_normals.size()>0){
	B_face_normals.clear();
    }

    Vec3f D_for_A=B->center-A->center;
    for(auto it=A_face_normals.begin();it!=A_face_normals.end();it++){
        //construct the four vertices of the plane
	Vec3f vertex1;
	Vec3f vertex2;
	Vec3f vertex3;
	Vec3f vertex4;
	Vec3f norm = (*(*it))*D_for_A>0 ? *(*it) : -(*(*it));

	if(*it==&(A->vec1)){
	    vertex1=A->a1*norm + A->a2*A->vec2 + A->a3*A->vec3;
	    vertex2=A->a1*norm - A->a2*A->vec2 + A->a3*A->vec3;
	    vertex3=A->a1*norm - A->a2*A->vec2 - A->a3*A->vec3;
	    vertex4=A->a1*norm - A->a2*A->vec2 + A->a3*A->vec3;
	}

	if(*it==&(A->vec2)){
	    vertex1=A->a1*A->vec1 + A->a2*norm + A->a3*A->vec3;
	    vertex2=A->a1*A->vec1 + A->a2*norm - A->a3*A->vec3;
	    vertex3=-A->a1*A->vec1 + A->a2*norm - A->a3*A->vec3;
	    vertex4=-A->a1*A->vec1 + A->a2*norm + A->a3*A->vec3;
	}

	if(*it==&(A->vec3)){
	    vertex1=A->a1*A->vec1 + A->a2*A->vec2 + A->a3*norm;
	    vertex2=A->a1*A->vec1 - A->a2*A->vec2 + A->a3*norm;
	    vertex3=-A->a1*A->vec1 - A->a2*A->vec2 + A->a3*norm;
	    vertex4=-A->a1*A->vec1 + A->a2*A->vec2 + A->a3*norm; 
	}

	Plane targetPlane(norm,vertex1);
	for(auto vIt=B->vertices.begin();vIt!=B->vertices.end();vIt++){
	    if(targetPlane.distance(*(*vIt))<0.05){
		Vec3f projectionPoint=targetPlane.projectionPt(*(*vIt));
		if(insideSquare(projectionPoint,vertex1,vertex2,vertex3,vertex4)){
		    returnVal.push_back(new Contact(B,A,*(*vIt),norm));
    		    //Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n) : 
		}
	    }
	}
    }

    Vec3f D_for_B=A->center-B->center;
    for(auto it=B_face_normals.begin();it!=B_face_normals.end();it++){
        //construct the four vertices of the plane
	Vec3f vertex1;
	Vec3f vertex2;
	Vec3f vertex3;
	Vec3f vertex4;
	Vec3f norm = (*(*it))*D_for_B>0 ? *(*it) : -(*(*it));

	if(*it==&(B->vec1)){
	    vertex1=B->a1*norm + B->a2*B->vec2 + B->a3*B->vec3;
	    vertex2=B->a1*norm - B->a2*B->vec2 + B->a3*B->vec3;
	    vertex3=B->a1*norm - B->a2*B->vec2 - B->a3*B->vec3;
	    vertex4=B->a1*norm - B->a2*B->vec2 + B->a3*B->vec3;
	}

	if(*it==&(B->vec2)){
	    vertex1=B->a1*B->vec1 + B->a2*norm + B->a3*B->vec3;
	    vertex2=B->a1*B->vec1 + B->a2*norm - B->a3*B->vec3;
	    vertex3=-B->a1*B->vec1 + B->a2*norm - B->a3*B->vec3;
	    vertex4=-B->a1*B->vec1 + B->a2*norm + B->a3*B->vec3;
	}

	if(*it==&(A->vec3)){
	    vertex1=B->a1*B->vec1 + B->a2*B->vec2 + B->a3*norm;
	    vertex2=B->a1*B->vec1 - B->a2*B->vec2 + B->a3*norm;
	    vertex3=-B->a1*B->vec1 - B->a2*B->vec2 + B->a3*norm;
	    vertex4=-B->a1*B->vec1 + B->a2*B->vec2 + B->a3*norm; 
	}

	Plane targetPlane(norm,vertex1);
	for(auto vIt=A->vertices.begin();vIt!=A->vertices.end();vIt++){
	    if(targetPlane.distance(*(*vIt))<0.05){
		Vec3f projectionPoint=targetPlane.projectionPt(*(*vIt));
		if(insideSquare(projectionPoint,vertex1,vertex2,vertex3,vertex4)){
		    returnVal.push_back(new Contact(A,B,*(*vIt),norm));
    		    //Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n) : 
		}
	    }
	}
    }

    //TODO: edge-edge
    Mat3f C(A->vec1*B->vec1, A->vec1*B->vec2, A->vec1*B->vec3,
	    A->vec2*B->vec1, A->vec2*B->vec2, A->vec2*B->vec3,
	    A->vec3*B->vec1, A->vec3*B->vec2, A->vec3*B->vec3);

    Vec3f D=B->center-A->center;
    Vec3f cross11=VectorOps::crossProduct(A->vec1,B->vec1);
    cross11.normalize();
    if(!A->checkIntersect(cross11,*B)){
	int sigma=cross11*D>0 ? 1 : -1;
	float x1=-sigma*pow(-1,negativeFlag(C[2][0]))*A->a2;
	float x2=sigma*pow(-1,negativeFlag(C[1][0]))*A->a3;
	float y1=-sigma*pow(-1,negativeFlag(C[0][2]))*B->a2;
	float y2=sigma*pow(-1,negativeFlag(C[0][1]))*B->a3;
	float x0=(1/(1-pow(C[0][0],2.0f)))*
		 (A->vec1*D+C[0][0]*(C[1][0]*x1+C[2][0]*x2-B->vec1*D)+C[0][1]*y1+C[0][2]*y2);
	
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross11,B->vec1,A->vec1));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }

    Vec3f cross12=VectorOps::crossProduct(A->vec1,B->vec2);
    cross12.normalize();
    if(!A->checkIntersect(cross12,*B)){
	int sigma=cross12*D>0 ? 1 : -1;
	float x1=-sigma*pow(-1,negativeFlag(C[2][1]))*A->a2;
	float x2=sigma*pow(-1,negativeFlag(C[1][1]))*A->a3;
	float y0=sigma*pow(-1,negativeFlag(C[0][2]))*B->a1;
	float y2=-sigma*pow(-1,negativeFlag(C[0][0]))*B->a3;
	float x0=(1/(1-pow(C[0][1],2.0f)))*
		 (A->vec1*D+C[0][1]*(C[1][1]*x1+C[2][1]*x2-B->vec2*D)+C[0][0]*y0+C[0][2]*y2);
	
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;

	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross12,B->vec2,A->vec1));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }

    Vec3f cross13=VectorOps::crossProduct(A->vec1,B->vec3);
    cross13.normalize();
    if(!A->checkIntersect(cross13,*B)){
	int sigma=cross13*D>0 ? 1 : -1;
	float x1=-sigma*pow(-1,negativeFlag(C[2][2]))*A->a2;
	float x2=sigma*pow(-1,negativeFlag(C[1][2]))*A->a3;
	float y0=-sigma*pow(-1,negativeFlag(C[0][1]))*B->a1;
	float y1=sigma*pow(-1,negativeFlag(C[0][0]))*B->a2;
	float x0=(1/(1-pow(C[0][2],2.0f)))*
		 (A->vec1*D+C[0][2]*(C[1][2]*x1+C[2][2]*x2-B->vec3*D)+C[0][0]*y0+C[0][1]*y1);
	
	//Vec3f pt(x0,x1,x2);
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross13,B->vec3,A->vec1));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }

    Vec3f cross21=VectorOps::crossProduct(A->vec2,B->vec1);
    cross21.normalize();
    if(!A->checkIntersect(cross21,*B)){
	int sigma=cross21*D>0 ? 1 : -1;
	float x0=sigma*pow(-1,negativeFlag(C[2][0]))*A->a1;
	float x2=-sigma*pow(-1,negativeFlag(C[0][0]))*A->a3;
	float y1=-sigma*pow(-1,negativeFlag(C[1][2]))*B->a2;
	float y2=sigma*pow(-1,negativeFlag(C[1][1]))*B->a3;
	float x1=(1/(1-pow(C[1][0],2.0f)))*
		 (A->vec2*D+C[1][0]*(C[0][0]*x0+C[2][0]*x2-B->vec1*D)+C[1][1]*y1+C[1][2]*y2);
	
	//Vec3f pt(x0,x1,x2);
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross21,B->vec1,A->vec2));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }

    Vec3f cross22=VectorOps::crossProduct(A->vec2,B->vec2);
    cross22.normalize();
    if(!A->checkIntersect(cross22,*B)){
	int sigma=cross22*D>0 ? 1 : -1;
	float x0=sigma*pow(-1,negativeFlag(C[2][1]))*A->a1;
	float x2=-sigma*pow(-1,negativeFlag(C[0][1]))*A->a3;
	float y0=sigma*pow(-1,negativeFlag(C[1][2]))*B->a1;
	float y2=-sigma*pow(-1,negativeFlag(C[1][0]))*B->a3;
	float x1=(1/(1-pow(C[1][1],2.0f)))*
		 (A->vec2*D+C[1][1]*(C[0][1]*x0+C[2][1]*x2-B->vec2*D)+C[1][0]*y0+C[1][2]*y2);
	
	//Vec3f pt(x0,x1,x2);
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross22,B->vec2,A->vec2));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }


    Vec3f cross23=VectorOps::crossProduct(A->vec2,B->vec3);
    cross23.normalize();
    if(!A->checkIntersect(cross23,*B)){
	int sigma=cross23*D>0 ? 1 : -1;
	float x0=sigma*pow(-1,negativeFlag(C[2][2]))*A->a1;
	float x2=-sigma*pow(-1,negativeFlag(C[0][2]))*A->a3;
	float y0=-sigma*pow(-1,negativeFlag(C[1][1]))*B->a1;
	float y1=sigma*pow(-1,negativeFlag(C[1][0]))*B->a2;
	float x1=(1/(1-pow(C[1][2],2.0f)))*
		 (A->vec2*D+C[1][2]*(C[0][2]*x0+C[2][2]*x2-B->vec3*D)+C[1][0]*y0+C[1][1]*y1);
	
	//Vec3f pt(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	cout<<"y1: "<<y1<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross23,B->vec3,A->vec2));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }


    Vec3f cross31=VectorOps::crossProduct(A->vec3,B->vec1);
    cross31.normalize();
    if(!A->checkIntersect(cross31,*B)){
	int sigma=cross31*D>0 ? 1 : -1;
	float x0=-sigma*pow(-1,negativeFlag(C[1][0]))*A->a1;
	float x1=sigma*pow(-1,negativeFlag(C[0][0]))*A->a2;
	float y1=-sigma*pow(-1,negativeFlag(C[2][2]))*B->a2;
	float y2=sigma*pow(-1,negativeFlag(C[2][1]))*B->a3;
	float x2=(1/(1-pow(C[2][0],2.0f)))*
		 (A->vec3*D+C[2][0]*(C[0][0]*x0+C[1][0]*x1-B->vec1*D)+C[2][1]*y1+C[2][2]*y2);
	
	//Vec3f pt(x0,x1,x2);
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross31,B->vec1,A->vec3));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }


    Vec3f cross32=VectorOps::crossProduct(A->vec3,B->vec2);
    cross32.normalize();
    if(!A->checkIntersect(cross32,*B)){
	int sigma=cross32*D>0 ? 1 : -1;
	float x0=-sigma*pow(-1,negativeFlag(C[1][1]))*A->a1;
	float x1=sigma*pow(-1,negativeFlag(C[0][1]))*A->a2;
	float y0=sigma*pow(-1,negativeFlag(C[2][2]))*B->a1;
	float y2=-sigma*pow(-1,negativeFlag(C[2][0]))*B->a3;
	float x2=(1/(1-pow(C[2][1],2.0f)))*
		 (A->vec3*D+C[2][1]*(C[0][1]*x0+C[1][1]*x1-B->vec2*D)+C[2][0]*y0+C[2][2]*y2);
	
	//Vec3f pt(x0,x1,x2);
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross32,B->vec2,A->vec3));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }


    Vec3f cross33=VectorOps::crossProduct(A->vec3,B->vec3);
    cross33.normalize();
    if(!A->checkIntersect(cross33,*B)){
	int sigma=cross33*D>0 ? 1 : -1;
	float x0=-sigma*pow(-1,negativeFlag(C[1][2]))*A->a1;
	float x1=sigma*pow(-1,negativeFlag(C[0][2]))*A->a2;
	float y0=-sigma*pow(-1,negativeFlag(C[2][1]))*B->a1;
	float y1=sigma*pow(-1,negativeFlag(C[2][0]))*B->a2;
	float x2=(1/(1-pow(C[2][2],2.0f)))*
		 (A->vec3*D+C[2][2]*(C[0][2]*x0+C[1][2]*x1-B->vec3*D)+C[2][0]*y0+C[2][1]*y1);
	
	//Vec3f pt(x0,x1,x2);
	//Vec3f pt=A->center+Vec3f(x0,x1,x2);
	cout<<"center: "<<A->center<<endl;
	cout<<"x0: "<<x0<<endl;
	cout<<"x1: "<<x1<<endl;
	cout<<"x2: "<<x2<<endl;
	Vec3f pt=A->center+x0*A->vec1+x1*A->vec2+x2*A->vec3;
	returnVal.push_back(new Contact(B,A,pt,sigma*cross33,B->vec3,A->vec3));

	//Contact(BoundingBox* a, BoundingBox* b, Vec3f p, Vec3f n, Vec3f ea, Vec3f eb) :
    }

    return returnVal;
}
*/
