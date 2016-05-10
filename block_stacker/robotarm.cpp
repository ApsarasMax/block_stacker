// The sample robotarm model.  You should build a file
// very similar to this for when you make your model.
#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#pragma warning(disable : 4786)
#pragma warning (disable : 4312)

#include "camera.h"
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "particleSystem.h"
#include "stone.h"
#include "modelerui.h"


#include <FL/Fl.H>
#include "mat.h"
#include <FL/gl.h>
#include <cstdlib>
#include <cfloat>
#include <vector>

using namespace std;


#define M_DEFAULT 2.0f
#define M_OFFSET 3.0f
#define P_OFFSET 0.3f
#define MAX_VEL 200
#define MIN_STEP 0.1

//parameters
float theta = 0.0;
float thetaMin=-180.0;
float thetaMax=180.0;

float phi = 55.0;

float psi = 30.0;
float psiMin=0.0;
float psiMax=135.0;

float cr = 0.0;
float crMin=-30.0;
float crMax=180.0;

float h1 = 0.8;
float h2 = 3.0;

float h3 = 2.5;
float h3Min=1.0;
float h3Max=20.0;

float pc = 5.0;

float hclaw = 0.5;
float hclawMin=0.3;
float hclawMax=FLT_MAX;


Vec3f magnetPos (0, 0, 0);

//must add stone on this region(variant)
bool dropOnGround = true;
float xMin=-2.5;
float xMax=2.5;
float zMin=-2.5;
float zMax=2.5;

float score = 0.0;



// This is a list of the controls for the RobotArm
// We'll use these constants to access the values 
// of the controls from the user interface.
enum RobotArmControls
{ 
    BASE_ROTATION=0, LOWER_TILT, UPPER_TILT, CLAW_ROTATION,
        BASE_LENGTH, LOWER_LENGTH, UPPER_LENGTH, PARTICLE_COUNT, NUMCONTROLS, 
};

void ground(float frameSLength);
void base(float h);
void rotation_base(float h);
void lower_arm(float h);
void upper_arm(float h);
void claw(float r, float h, float phi, float psi, float cr);
void y_box(float h);
void stone(float x, float y, float z, float sLength);
bool inRightPosition(Vec3f pos,float length);
Mat4f glGetMatrix(GLenum pname);
Vec3f getWorldPoint(Mat4f matCamXforms);

// To make a RobotArm, we inherit off of ModelerView
class RobotArm : public ModelerView 
{
private:
    vector<Stone*> stoneVec;
    Stone* targetStone;
    bool isHooked;
public:
    RobotArm(int x, int y, int w, int h, char *label) 
        : ModelerView(x,y,w,h,label) {
	
	isHooked=false;
	targetStone=0;

	//TODO: make colors and positions random
    Vec3f color0 ( 0.45, 0.45, 0.45 );
	Vec3f color1 ( 0.35, 0.35, 0.35 );
	Vec3f color2 ( 0.45, 0.45, 0.45 );
	Vec3f color3 ( 0.55, 0.55, 0.55 );
	Vec3f color4 ( 0.65, 0.65, 0.65 );

	stoneVec.push_back(new Stone( 3.5, 1.2, 1, 1.2, color0));
	stoneVec.push_back(new Stone( -5, 1.2, 3, 1.2, color1));
	stoneVec.push_back(new Stone( -2, 1.2, 4, 1.2, color2));
	stoneVec.push_back(new Stone( -6, 1.2, -4.5, 1.2, color3));
	stoneVec.push_back(new Stone(  2, 1.2, 4, 1.2, color4));

    }

    float getAltitude(Vec3f magPos);
    virtual void draw();
    int handle(int event);
    Vec3f updateMagnetPos();
    void update(float& qName,float min, float max, float stepSize);
    void reset();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createRobotArm(int x, int y, int w, int h, char *label)
{ 
    return new RobotArm(x,y,w,h,label); 
}

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))


// Utility function.  Use glGetMatrix(GL_MODELVIEW_MATRIX) to retrieve
//  the current ModelView matrix.
Mat4f glGetMatrix(GLenum pname){
    GLfloat m[16];
    glGetFloatv(pname, m);
    Mat4f matCam(m[0],  m[1],  m[2],  m[3],
                            m[4],  m[5],  m[6],  m[7],
                            m[8],  m[9],  m[10], m[11],
                            m[12], m[13], m[14], m[15] );

    // because the matrix GL returns is column major...
    return matCam.transpose();
}

bool inRightPosition(Vec3f pos,float length){
	if(dropOnGround){
	   dropOnGround = false;
		return pos[0]<xMax-length/2.0 && pos[0]>xMin+length/2.0 &&
	   pos[2]<zMax-length/2.0 && pos[2]>zMin+length/2.0;
	}else{
		return pos[0]<xMax && pos[0]>xMin &&
	   pos[2]<zMax && pos[2]>zMin;
	}
}

void RobotArm::reset(){
    theta = 0.0;
    phi = 55.0;
    psi = 30.0;
    cr = 0.0;
    crMin=-30.0;
    crMax=180.0;
    h1 = 0.8;
    h2 = 3.0;
    h3 = 2.5;
    pc = 5.0;
    hclaw = 0.5;

    isHooked=false;
    targetStone=0;

    dropOnGround = true;
	xMin=-2.5;
	xMax=2.5;
	zMin=-2.5;
	zMax=2.5;

	score = 0.0;

    Vec3f color0 ( 0.45, 0.45, 0.45 );
    Vec3f color1 ( 0.35, 0.35, 0.35 );
    Vec3f color2 ( 0.45, 0.45, 0.45 );
    Vec3f color3 ( 0.55, 0.55, 0.55 );
    Vec3f color4 ( 0.65, 0.65, 0.65 );

    stoneVec.clear();

    stoneVec.push_back(new Stone( 3.5, 1.2, 1, 1.2, color0));
    stoneVec.push_back(new Stone( -5, 1.2, 3, 1.2, color1));
    stoneVec.push_back(new Stone( -2, 1.2, 4, 1.2, color2));
    stoneVec.push_back(new Stone( -6, 1.2, -4.5, 1.2, color3));
    stoneVec.push_back(new Stone( 2, 1.2, 4, 1.2, color4));
    magnetPos=updateMagnetPos();

}

void RobotArm::update(float& qName,float min, float max, float stepSize){
    
	//experiment//zyc
	Fl_Text_Buffer *buffer=ModelerApplication::Instance()->GetUI()->m_pwndTxtBuf;
	// char text[200];
	// strcpy (text,buffer->text());
	// strcat (text,"strings \n");
	// //cout<<text<<endl;
	// buffer->text(text);

    float oldQName=qName;
    if(qName+stepSize>max){
	    qName=max;
    }else{
		if(qName+stepSize<min){
		    qName=min;
		}else{
		    qName+=stepSize;
		}
    }
    Vec3f magPos=updateMagnetPos();
    float deltaX=magPos[0]-magnetPos[0];
    float deltaY=magPos[1]-magnetPos[1];
    float deltaZ=magPos[2]-magnetPos[2];

    //cout<<"getAltitude(magPos): "<<getAltitude(magPos)<<endl;
    
    if(isHooked){
		if(targetStone->getBottomHeight()+deltaY>getAltitude(magPos)){
		    Vec3f pos=targetStone->getPosition();
		    targetStone->setX(pos[0]+deltaX);
		    targetStone->setY(pos[1]+deltaY);
		    targetStone->setZ(pos[2]+deltaZ);
		}else{
		    if(inRightPosition(targetStone->getPosition(),targetStone->getSLength())){
		        isHooked=false;
				targetStone->setIsInPosition(true);
		        targetStone->setIsOnMagnet(false);

		        //set for the right region(can only add on stone, not square)
		        xMin = targetStone->getPosition()[0] - targetStone->getSLength();
		        xMax = targetStone->getPosition()[0] + targetStone->getSLength();
		        zMin = targetStone->getPosition()[2] - targetStone->getSLength();
		        zMax = targetStone->getPosition()[2] + targetStone->getSLength();

		        score += targetStone->getSLength();

		    	char text[500];
				strcpy (text,buffer->text());
				strcat (text,"\t...placed...\n");
				strcat (text,"\tcurrent score: ");
				char foo[10];
				sprintf(foo, "%.2f\n\n", score);
				strcat (text,foo);
				//strcat (text,"\tcurrent score: 01 \n\n");
				buffer->text(text);

		        targetStone=0;
		    }
		    qName=oldQName;
		    return;
		}
    }else{
		if(magPos[1]-getAltitude(magPos)<FLT_EPSILON){
		    if(targetStone && !targetStone->getIsInPosition()){
		        isHooked=true;
		        targetStone->setIsOnMagnet(true);

		    	char text[200];
				strcpy (text,buffer->text());
				strcat (text,"\t...hooked...\n");
				buffer->text(text);
		    }
		    qName=oldQName;
		    return;
		}
    }

    magnetPos=magPos;
    //delete text;
}

Vec3f RobotArm::updateMagnetPos(){
    Mat4f matCam = glGetMatrix( GL_MODELVIEW_MATRIX );
    Mat4f matCamInverse = matCam.inverse();
	
	glPushMatrix();

		glTranslatef( 5.0, 0.0, -5.0 );	
		glRotatef( 45, 0.0, 1.0, 0.0 );


	    glTranslatef( 0.0, 0.8, 0.0 );			// move to the top of the base
	    glRotatef( theta, 0.0, 1.0, 0.0 );		// turn the whole assembly around the y-axis. 

	    glTranslatef( 0.0, h1, 0.0 );			// move to the top of the base
	    glRotatef( phi, 0.0, 0.0, 1.0 );		// rotate around the z-axis for the lower arm
		glTranslatef( -0.1, 0.0, 0.4 );

	    glTranslatef( 0.0, h2, 0.0 );			// move to the top of the lower arm
	    glRotatef( psi, 0.0, 0.0, 1.0 );		// rotate  around z-axis for the upper arm

		glTranslatef( 0.0, h3, 0.0 );
		glRotatef( cr, 0.0, 0.0, 1.0 );

		{
			glTranslatef(0, 0.25, -0.2502);

			glTranslatef(-0.25, 0.0, 0.0);
			glRotatef( -phi, 0.0, 0.0, 1.0 );
			glRotatef( -psi, 0.0, 0.0, 1.0 );
			glRotatef( -cr, 0.0, 0.0, 1.0 );

			glTranslatef(0.05, -hclaw, 0.2502);
			glRotatef( -90, 0.0, 1.0, 0.0 );

			glTranslatef(0, -0.2, 0.05);
			glRotatef( 90, 1.0, 0.0, 0.0 );

			glTranslatef(0, 0, 0.1);
		}


		Mat4f matNew = glGetMatrix(GL_MODELVIEW_MATRIX);

	glPopMatrix();
		Mat4f matBase = matCamInverse * matNew;

	return matBase * Vec4f(0.0, 0.0, 0.0, 1.0);
}

float RobotArm::getAltitude(Vec3f magPos){
    float maxHeight=0.0f;
    Stone* maxStone=0;

    for(auto it=stoneVec.begin();it!=stoneVec.end();it++){
		if((*it)->inStone(magPos[0],magPos[2])){
		    if((*it)->getTopHeight()>maxHeight && (*it)!=maxStone && !(*it)->getIsOnMagnet()){
				maxHeight=(*it)->getTopHeight();
				maxStone=*it;
		    }
		}
    }
    if(!isHooked)
        targetStone=maxStone;
    return maxHeight;
}

int RobotArm::handle(int event)
{

	switch(event) 
	{
	case FL_SHORTCUT: 
		{
			//cout<<Fl::event_key()<<endl;
			switch(Fl::event_key())
			{
				case 97: update(theta,thetaMin,thetaMax,0.5);	break;//a
				case 100: update(theta,thetaMin,thetaMax,-0.5);	break;//d
				case 119: update(hclaw,hclawMin,hclawMax, -0.05); break;//w
				case 115: update(hclaw,hclawMin,hclawMax,0.05);   break;//s
				case 49: update(phi,psiMin,psiMax,-0.3);	break;//1
				case 50: update(phi,psiMin,psiMax,0.3);	break;//2
				case 51: update(h3,h3Min,h3Max,0.05);	break;//3
				case 52: update(h3,h3Min,h3Max,-0.05);	break;//4
				case 53: update(psi,psiMin,psiMax,-0.3);	break;//5
				case 54: update(psi,psiMin,psiMax,0.3);	break;//6
				case 55: update(cr,crMin,crMax,1.0);	break;//7
				case 56: update(cr,crMin,crMax,-1.0);	break;//8
				case 65307: reset();    break;//Esc
				default: return ModelerView::handle(event);
			}
		}
		break;
	
	default:
		return ModelerView::handle(event);
	}
	
	redraw();

	return 1;
}


// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out RobotArm
void RobotArm::draw()
{
	/* pick up the slider values */

    // This call takes care of a lot of the nasty projection 
    // matrix stuff
    ModelerView::draw();

    // Save the camera transform that was applied by
    // ModelerView::draw() above.
    // While we're at it, save an inverted copy of this matrix.  We'll
    // need it later.
    Mat4f matCam = glGetMatrix( GL_MODELVIEW_MATRIX );
    Mat4f matCamInverse = matCam.inverse();


	static GLfloat lmodel_ambient[] = {0.4,0.4,0.4,1.0};

	// define the model

	
	glPushMatrix();

		ground(5);
		glTranslatef( 5.0, 0.0, -5.0 );	
		glRotatef( 45, 0.0, 1.0, 0.0 );

		base(0.8);

	    glTranslatef( 0.0, 0.8, 0.0 );			// move to the top of the base
	    glRotatef( theta, 0.0, 1.0, 0.0 );		// turn the whole assembly around the y-axis. 
		rotation_base(h1);						// draw the rotation base

	    glTranslatef( 0.0, h1, 0.0 );			// move to the top of the base
		//glPushMatrix();
		//		glTranslatef( 0.5, h1, 0.6 );	
		//glPopMatrix();
	    glRotatef( phi, 0.0, 0.0, 1.0 );		// rotate around the z-axis for the lower arm
		glTranslatef( -0.1, 0.0, 0.4 );
		lower_arm(h2);							// draw the lower arm

	    glTranslatef( 0.0, h2, 0.0 );			// move to the top of the lower arm
	    glRotatef( psi, 0.0, 0.0, 1.0 );		// rotate  around z-axis for the upper arm
		upper_arm(h3);							// draw the upper arm

		glTranslatef( 0.0, h3, 0.0 );
		glRotatef( cr, 0.0, 0.0, 1.0 );

		claw(0.5, hclaw, phi, psi, cr);

		{
			glTranslatef(0, 0.25, -0.2502);

			glTranslatef(-0.25, 0.0, 0.0);
			glRotatef( -phi, 0.0, 0.0, 1.0 );
			glRotatef( -psi, 0.0, 0.0, 1.0 );
			glRotatef( -cr, 0.0, 0.0, 1.0 );

			glTranslatef(0.05, -hclaw, 0.2502);
			glRotatef( -90, 0.0, 1.0, 0.0 );

			glTranslatef(0, -0.2, 0.05);
			glRotatef( 90, 1.0, 0.0, 0.0 );

			glTranslatef(0, 0, 0.1);
		}


		Mat4f matNew = glGetMatrix(GL_MODELVIEW_MATRIX);

	glPopMatrix();
		Mat4f matBase = matCamInverse * matNew;

	magnetPos = matBase * Vec4f(0.0, 0.0, 0.0, 1.0);
		
	glPushMatrix();
		setAmbientColor( 0.25, 0.25, 0.65 );	

		for(auto it=stoneVec.begin();it!=stoneVec.end();it++){
		    (*it)->drawStone();
		}

	glPopMatrix();




	//*** DON'T FORGET TO PUT THIS IN YOUR OWN CODE **/
	endDraw();

}

void square(float frameSLength){
	glBegin(GL_POLYGON);
		glColor3f(0.65,0.85,0.85);
		glVertex3d( frameSLength / 2.0,0.0, frameSLength / 2.0);
		glVertex3d(-frameSLength / 2.0,0.0, frameSLength / 2.0);
		glVertex3d(-frameSLength / 2.0,0.0,-frameSLength / 2.0);
		glVertex3d(frameSLength / 2.0,0.0,-frameSLength / 2.0);
	glEnd();
}

void ground(float frameSLength){
	glDisable(GL_LIGHTING);
	glColor3f(0.65,0.45,0.4);
	glPushMatrix();
		glScalef(30,0,30);
		y_box(1);
	glPopMatrix();
	glPushMatrix();
		glTranslatef( 0, 0.05, 0 );
		square(frameSLength);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void base(float h) {
	setDiffuseColor( 0.25, 0.25, 0.25 );
	setAmbientColor( 0.25, 0.25, 0.25 );
	glPushMatrix();
		glPushMatrix();
			glTranslatef(1.0, h / 2.0, 0.75);
			drawCylinder(0.25, h / 2.0, h / 2.0);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.0, h / 2.0, -1.0);
			drawCylinder(0.25, h / 2.0, h / 2.0);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-1.0, h / 2.0, 0.75);
			drawCylinder(0.25, h / 2.0, h / 2.0);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-1.0, h / 2.0, -1.0);
			drawCylinder(0.25, h / 2.0, h / 2.0);
		glPopMatrix();
	glScalef(4.0f, h, 4.0f);
	y_box(1.0f);
	glPopMatrix();
}

void rotation_base(float h) {
	setDiffuseColor( 0.85, 0.75, 0.25 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	glPushMatrix();
		glPushMatrix();
			glScalef(4.0, h, 4.0);
			y_box(1.0f); // the rotation base
		glPopMatrix();
		setDiffuseColor( 0.15, 0.15, 0.65 );
		setAmbientColor( 0.15, 0.15, 0.65 );
		glPushMatrix();
			glTranslatef(-0.5, h, -0.6);
			glScalef(2.0, h, 1.6);
			y_box(1.0f); // the console
		glPopMatrix();
		setDiffuseColor( 0.65, 0.65, 0.65 );
		setAmbientColor( 0.65, 0.65, 0.65 );
		glPushMatrix();
			glTranslatef( 0.5, h, 0.6 );
			glRotatef( -90.0, 1.0, 0.0, 0.0 );
			drawCylinder( h, 0.05, 0.05 ); // the pipe
		glPopMatrix();
	glPopMatrix();
}

void lower_arm(float h) {					// draw the lower arm
	setDiffuseColor( 0.85, 0.75, 0.25 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	y_box(h);
}

void upper_arm(float h) {					// draw the upper arm
	setDiffuseColor( 0.85, 0.75, 0.25 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	glPushMatrix();
	glScalef( 1.0, 1.0, 0.7 );
	y_box(h);
	glPopMatrix();
}

void claw(float r, float h, float phi, float psi, float cr) {
	setDiffuseColor( 0.5, 0.5, 0.5 );
	setAmbientColor( 0.25, 0.25, 0.85 );

	glPushMatrix();
		glTranslatef(0, r / 2.0, -0.25);
		drawCylinder(0.5, r / 2.0, r / 2.0);
	glPopMatrix();

	glBegin( GL_QUADS );

	glNormal3d( 1.0,  0.0,  0.0);	// +x side
	glVertex3d( 0.25, 0.0,-0.25);
	glVertex3d( 0.25, 0.0, 0.25);
	glVertex3d( 0.25,   r/2.0, 0.25);
	glVertex3d( 0.25,   r/2.0,-0.25);

	glNormal3d( -1.0,  0.0,  0.0);	// -x side
	glVertex3d( -0.25, 0.0,-0.25);
	glVertex3d( -0.25, 0.0, 0.25);
	glVertex3d( -0.25,   r/2.0, 0.25);
	glVertex3d( -0.25,   r/2.0,-0.25);

	glNormal3d( 0.0,-1.0, 0.0);		// -y side
	glVertex3d( 0.25, 0.0, 0.25);
	glVertex3d( 0.25, 0.0,-0.25);
	glVertex3d(-0.25, 0.0,-0.25);
	glVertex3d(-0.25, 0.0, 0.25);

	glNormal3d( 0.0,  0.0,  1.0);	// z side
	glVertex3d( -0.25, 0.0, 0.2501);
	glVertex3d( 0.25, 0.0, 0.2501);
	glVertex3d( 0.25,   r/2.0, 0.2501);
	glVertex3d( -0.25,   r/2.0, 0.2501);

	glNormal3d( 0.0,  0.0,  -1.0);	// -z side
	glVertex3d( -0.25, 0.0,-0.2501);
	glVertex3d( 0.25, 0.0,-0.2501);
	glVertex3d( 0.25,   r/2.0,-0.2501);
	glVertex3d( -0.25,   r/2.0,-0.2501);

	glEnd();

	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(0, 0.25, 0.2502);
		glLineWidth(2); 
		glColor3f(0, 0.8, 0.8);
		glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(-0.25, 0, 0);
		glEnd();
		glTranslatef(-0.25, 0.0, 0.0);
		glRotatef( -phi, 0.0, 0.0, 1.0 );
		glRotatef( -psi, 0.0, 0.0, 1.0 );
		glRotatef( -cr, 0.0, 0.0, 1.0 );
		glBegin(GL_LINES);			
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0-h, -0.0502);
		glEnd();

	glPopMatrix();
	glEnable(GL_LIGHTING);

	glPushMatrix();
		glTranslatef(0, 0.25, -0.2502);
		glLineWidth(2); 

		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(-0.25, 0, 0);
		glEnd();
		glTranslatef(-0.25, 0.0, 0.0);
		glRotatef( -phi, 0.0, 0.0, 1.0 );
		glRotatef( -psi, 0.0, 0.0, 1.0 );
		glRotatef( -cr, 0.0, 0.0, 1.0 );
		glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0-h, 0.0502);
		glEnd();
		
		glEnable(GL_LIGHTING);
		
	glTranslatef(0.05, -h, 0.2502);
	glRotatef( -90, 0.0, 1.0, 0.0 );
	setDiffuseColor( 0, 0.8, 0.8 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	drawCylinder(0.10, 0.2, 0.2);

	glTranslatef(0, -0.2, 0.05);
	glRotatef( 90, 1.0, 0.0, 0.0 );
	setDiffuseColor( 0.8, 0.8, 0.8 );
	setAmbientColor( 0.95, 0.75, 0.25 );
	drawCylinder(0.1, 0.05, 0.05);

	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void y_box(float h) {

	glBegin( GL_QUADS );

	glNormal3d( 1.0 ,0.0, 0.0);			// +x side
	glVertex3d( 0.25,0.0, 0.25);
	glVertex3d( 0.25,0.0,-0.25);
	glVertex3d( 0.25,  h,-0.25);
	glVertex3d( 0.25,  h, 0.25);

	glNormal3d( 0.0 ,0.0, -1.0);		// -z side
	glVertex3d( 0.25,0.0,-0.25);
	glVertex3d(-0.25,0.0,-0.25);
	glVertex3d(-0.25,  h,-0.25);
	glVertex3d( 0.25,  h,-0.25);

	glNormal3d(-1.0, 0.0, 0.0);			// -x side
	glVertex3d(-0.25,0.0,-0.25);
	glVertex3d(-0.25,0.0, 0.25);
	glVertex3d(-0.25,  h, 0.25);
	glVertex3d(-0.25,  h,-0.25);

	glNormal3d( 0.0, 0.0, 1.0);			// +z side
	glVertex3d(-0.25,0.0, 0.25);
	glVertex3d( 0.25,0.0, 0.25);
	glVertex3d( 0.25,  h, 0.25);
	glVertex3d(-0.25,  h, 0.25);

	glNormal3d( 0.0, 1.0, 0.0);			// top (+y)
	glVertex3d( 0.25,  h, 0.25);
	glVertex3d( 0.25,  h,-0.25);
	glVertex3d(-0.25,  h,-0.25);
	glVertex3d(-0.25,  h, 0.25);

	glNormal3d( 0.0,-1.0, 0.0);			// bottom (-y)
	glVertex3d( 0.25,0.0, 0.25);
	glVertex3d(-0.25,0.0, 0.25);
	glVertex3d(-0.25,0.0,-0.25);
	glVertex3d( 0.25,0.0,-0.25);

	glEnd();
}

void stone(float x, float y, float z, float sLength) {

	glBegin( GL_QUADS );

	glNormal3d( 1.0 ,0.0, 0.0);			// +x side
	glVertex3d( x + sLength / 2.0, y - sLength, z + sLength / 2.0);
	glVertex3d( x + sLength / 2.0, y - sLength, z - sLength / 2.0);
	glVertex3d( x + sLength / 2.0,  y, z - sLength / 2.0);
	glVertex3d( x + sLength / 2.0,  y, z + sLength / 2.0);

	glNormal3d( 0.0 ,0.0, -1.0);		// -z side
	glVertex3d( x + sLength / 2.0, y - sLength, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0, y - sLength, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y, z - sLength / 2.0);
	glVertex3d( x + sLength / 2.0,  y, z - sLength / 2.0);

	glNormal3d(-1.0, 0.0, 0.0);			// -x side
	glVertex3d( x - sLength / 2.0, y - sLength, z + sLength / 2.0);
	glVertex3d( x - sLength / 2.0, y - sLength, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y, z + sLength / 2.0);

	glNormal3d( 0.0, 0.0, 1.0);			// +z side
	glVertex3d( x + sLength / 2.0, y - sLength, z + sLength / 2.0);
	glVertex3d( x - sLength / 2.0, y - sLength, z + sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y, z + sLength / 2.0);
	glVertex3d( x + sLength / 2.0,  y, z + sLength / 2.0);

	glNormal3d( 0.0, 1.0, 0.0);			// top (+y)
	glVertex3d( x + sLength / 2.0,  y, z + sLength / 2.0);
	glVertex3d( x + sLength / 2.0,  y, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y, z + sLength / 2.0);

	glNormal3d( 0.0,-1.0, 0.0);			// bottom (-y)
	glVertex3d( x + sLength / 2.0,  y - sLength, z + sLength / 2.0);
	glVertex3d( x + sLength / 2.0,  y - sLength, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y - sLength, z - sLength / 2.0);
	glVertex3d( x - sLength / 2.0,  y - sLength, z + sLength / 2.0);

	glEnd();
}

int main()
{
    
	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.

    ModelerApplication::Instance()->Init(&createRobotArm);
    return ModelerApplication::Instance()->Run();
}
