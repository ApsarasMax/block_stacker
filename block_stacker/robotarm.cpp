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


#include <FL/Fl.H>
#include "mat.h"
#include <FL/gl.h>
#include <cstdlib>

using namespace std;

#define M_DEFAULT 2.0f
#define M_OFFSET 3.0f
#define P_OFFSET 0.3f
#define MAX_VEL 200
#define MIN_STEP 0.1

float theta = 0.0;
float phi = 55.0;
float psi = 30.0;
float cr = 0.0;
float h1 = 0.8;
float h2 = 3.0;
float h3 = 2.5;
float pc = 5.0;
float hclaw = 0.5;

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
Mat4f glGetMatrix(GLenum pname);
Vec3f getWorldPoint(Mat4f matCamXforms);

// To make a RobotArm, we inherit off of ModelerView
class RobotArm : public ModelerView 
{
public:
    RobotArm(int x, int y, int w, int h, char *label) 
        : ModelerView(x,y,w,h,label) {}
    virtual void draw();
    int handle(int event);
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
Mat4f glGetMatrix(GLenum pname)
{
    GLfloat m[16];
    glGetFloatv(pname, m);
    Mat4f matCam(m[0],  m[1],  m[2],  m[3],
                            m[4],  m[5],  m[6],  m[7],
                            m[8],  m[9],  m[10], m[11],
                            m[12], m[13], m[14], m[15] );

    // because the matrix GL returns is column major...
    return matCam.transpose();
}

int RobotArm::handle(int event)
{

	switch(event)	 
	{
	case FL_SHORTCUT: 
		{
			switch(Fl::event_key())
			{
				case 97: theta += 0.5;	break;//a
				case 100: theta -= 0.5;	break;//d
				case 119: h2 += 0.05;	break;//w
				case 115: h2 -= 0.05;	break;//s
				case 114: psi += 0.3;	break;//r
				case 102: psi -= 0.3;	break;//f
				case 116: cr += 1.0;	break;//t
				case 103: cr -= 1.0;	break;//g
				case 65362: hclaw -= 0.05;	break;//up
				case 65364: hclaw += 0.05;	break;//down
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
		setDiffuseColor( 0.25, 0.25, 0.25 );
		setAmbientColor( 0.25, 0.25, 0.25 );
		stone(3.5, 1.2, 1, 1.2);

		setDiffuseColor( 0.35, 0.35, 0.35 );
		stone(-5, 1.2, 3, 1.2);

		setDiffuseColor( 0.45, 0.45, 0.45 );
		stone(-2, 1.2, 4, 1.2);

		setDiffuseColor( 0.55, 0.55, 0.55 );
		stone(-6, 1.2, -4.5, 1.2);

		setDiffuseColor( 0.65, 0.65, 0.65 );
		stone(2, 1.2, 4, 1.2);
	glPopMatrix();

	glPushMatrix();

		ground(5);
		glTranslatef( 5.0, 0.0, -5.0 );	
		glRotatef( 45, 0.0, 1.0, 0.0 );

		base(0.8);

	    glTranslatef( 0.0, 0.8, 0.0 );			// move to the top of the base
	    glRotatef( theta, 0.0, 1.0, 0.0 );		// turn the whole assembly around the y-axis. 
		rotation_base(h1);						// draw the rotation base

	    glTranslatef( 0.0, h1, 0.0 );			// move to the top of the base
		glPushMatrix();
				glTranslatef( 0.5, h1, 0.6 );	
		glPopMatrix();
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
		}


		Mat4f matNew = glGetMatrix(GL_MODELVIEW_MATRIX);

	glPopMatrix();
		Mat4f matBase = matCamInverse * matNew;

	Vec3f pos = matBase * Vec4f(0.0, 0.0, 0.0, 1.0);

	cout<<pos<<endl;




	//*** DON'T FORGET TO PUT THIS IN YOUR OWN CODE **/
	endDraw();
}

void square(float frameSLength){
	glLineWidth(1); 
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.4f, 0.4f);
		glVertex3d( frameSLength / 2.0,0.0, frameSLength / 2.0);
		glVertex3d(-frameSLength / 2.0,0.0, frameSLength / 2.0);

		glVertex3d(-frameSLength / 2.0,0.0,-frameSLength / 2.0);
		glVertex3d( frameSLength / 2.0,0.0,-frameSLength / 2.0);

		glVertex3d( frameSLength / 2.0,0.0, frameSLength / 2.0);
		glVertex3d( frameSLength / 2.0,0.0,-frameSLength / 2.0);

		glVertex3d(-frameSLength / 2.0,0.0,-frameSLength / 2.0);
		glVertex3d(-frameSLength / 2.0,0.0, frameSLength / 2.0);

	glEnd();
}

void ground(float frameSLength) 
{
	glDisable(GL_LIGHTING);
	glColor3f(0.65,0.45,0.2);
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

	glNormal3d( 1.0,  0.0,  0.0);	// z side
	glVertex3d( -0.25, 0.0, 0.2501);
	glVertex3d( 0.25, 0.0, 0.2501);
	glVertex3d( 0.25,   r/2.0, 0.2501);
	glVertex3d( -0.25,   r/2.0, 0.2501);

	glNormal3d( -1.0,  0.0,  0.0);	// -z side
	glVertex3d( -0.25, 0.0,-0.2501);
	glVertex3d( 0.25, 0.0,-0.2501);
	glVertex3d( 0.25,   r/2.0,-0.2501);
	glVertex3d( -0.25,   r/2.0,-0.2501);

	glEnd();

	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(0, 0.25, 0.2502);
		glLineWidth(2); 

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
			glVertex3f(0.0, 0.0-h, -0.0502);//x need to ba a para
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
			glVertex3f(0.0, 0.0-h, 0.0502);//x need to ba a para
		glEnd();
		glEnable(GL_LIGHTING);

	glTranslatef(0.05, -h, 0.2502);
	glRotatef( -90, 0.0, 1.0, 0.0 );
	setDiffuseColor( 0, 0.5, 0.5 );
	drawCylinder(0.10, 0.2, 0.2);

	glTranslatef(0, -0.2, 0.05);
	glRotatef( 90, 1.0, 0.0, 0.0 );
	setDiffuseColor( 0.2, 0.2, 0.2 );
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
    ModelerControl controls[NUMCONTROLS ];

	controls[BASE_ROTATION] = ModelerControl("base rotation (theta)", -180.0, 180.0, 0.1, 0.0 );
    controls[LOWER_TILT] = ModelerControl("lower arm tilt (phi)", 15.0, 95.0, 0.1, 55.0 );
    controls[UPPER_TILT] = ModelerControl("upper arm tilt (psi)", 0.0, 135.0, 0.1, 30.0 );
	controls[CLAW_ROTATION] = ModelerControl("claw rotation (cr)", -30.0, 180.0, 0.1, 0.0 );
    controls[BASE_LENGTH] = ModelerControl("base height (h1)", 0.5, 10.0, 0.1, 0.8 );
    controls[LOWER_LENGTH] = ModelerControl("lower arm length (h2)", 1, 10.0, 0.1, 3.0 );
    controls[UPPER_LENGTH] = ModelerControl("upper arm length (h3)", 1, 10.0, 0.1, 2.5 );
    controls[PARTICLE_COUNT] = ModelerControl("particle count (pc)", 0.0, 5.0, 0.1, 5.0 );
    

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.

    ModelerApplication::Instance()->Init(&createRobotArm, controls, NUMCONTROLS);
    return ModelerApplication::Instance()->Run();
}
