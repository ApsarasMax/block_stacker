#ifdef WIN32
#include <windows.h>
#endif
#include <FL/gl.h>
#include <FL/glu.h>
#include <fstream>
#include <cstdio>

#include "camera.h"
#include "modelerapp.h"	// needed to read values from graph widget
#include "modelerui.h"	// needed to read values from graph widget

#include <FL/Fl.H>

#pragma warning(push)
#pragma warning(disable : 4244)

// As of 02sp, the camera curves appear in the "model controls" pane.  
//#define CAM_VAL(x) (ModelerApplication::Instance()->GetControlValue(GetCamCurveIndex(x)))

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502f
#endif 

const float kMouseRotationSensitivity		= 1.0f/90.0f;
const float kMouseTranslationXSensitivity	= 0.03f;
const float kMouseTranslationYSensitivity	= 0.03f;
const float kMouseZoomSensitivity			= 0.08f;


Camera::Camera() 
{
	mAzimuth = mTwist = 0.0f;
	mElevation = 0.7f;
	mDolly = -30.0f;
	mLookAt = Vec3f( 0, 0, 0 );
	mFOV = 30;

	mCurrentMouseAction = kActionNone;

	calculateViewingTransformParameters();
}

/** 
 * Possibly useful fxn that is not currently called.
 * You could use this to generate the camera's up and right
 * vectors in world space...
 */
void MakeCamTrans(Mat4f &m, Vec3f &eye, 
					Vec3f &at, Vec3f &up)
{

	// compute normalized vectors
	Vec3f n_up = up;
	n_up.normalize();
	Vec3f k = (at - eye);
	k.normalize();

	// compute orthonormal basis (i, j, k)
	Vec3f i = k ^ n_up;
	Vec3f j = i ^ k;
	i.normalize();
	j.normalize();

	// construct rotation matrix
	m = Mat4f(i[0], i[1], i[2], 0,
			  j[0], j[1], j[2], 0,
			  -k[0], -k[1], -k[2], 0,
			  0,0,0,1);
}


void Camera::calculateViewingTransformParameters() 
{
	// compute new transformation based on
	// user interaction
	Mat4f dollyXform;
	Mat4f azimXform;
	Mat4f elevXform;
	Mat4f twistXform;
	Mat4f originXform;

	Vec3f upVector;

	makeHTrans(dollyXform, Vec3f(0,0,mDolly));
	makeHRotY(azimXform, mAzimuth);
	makeHRotX(elevXform, mElevation);
	makeDiagonal(twistXform, 1.0f);
	makeHTrans(originXform, mLookAt);
	
	mPosition = Vec3f(0,0,0);
	// grouped for (mat4 * vec3) ops instead of (mat4 * mat4) ops
	mPosition = originXform * (azimXform * (elevXform * (dollyXform * mPosition)));

	if ( fmodf(mElevation, 2.0*M_PI) < 3*M_PI/2 && fmodf(mElevation, 2.0*M_PI) > M_PI/2 )
		mUpVector= Vec3f(0,-1,0);
	else
		mUpVector= Vec3f(0,1,0);

	mDirtyTransform = false;
}

void Camera::clickMouse( MouseAction_t action, int x, int y )
{
	mCurrentMouseAction = action;
	mLastMousePosition[0] = x;
	mLastMousePosition[1] = y;
}

void Camera::dragMouse( int x, int y )
{
	Vec3f mouseDelta   = Vec3f(x,y,0.0f) - mLastMousePosition;
	mLastMousePosition = Vec3f(x,y,0.0f);

	switch(mCurrentMouseAction)
	{
	case kActionTranslate:
		{
			calculateViewingTransformParameters();

			double xTrack =  -mouseDelta[0] * kMouseTranslationXSensitivity;
			double yTrack =  mouseDelta[1] * kMouseTranslationYSensitivity;

			Vec3f transXAxis = mUpVector ^ (mPosition - mLookAt);
			transXAxis /= sqrt((transXAxis*transXAxis));
			Vec3f transYAxis = (mPosition - mLookAt) ^ transXAxis;
			transYAxis /= sqrt((transYAxis*transYAxis));

			setLookAt(getLookAt() + transXAxis*xTrack + transYAxis*yTrack);
			
			break;
		}
	case kActionRotate:
		{
			float dAzimuth		=   -mouseDelta[0] * kMouseRotationSensitivity;
			float dElevation	=   mouseDelta[1] * kMouseRotationSensitivity;
			
			setAzimuth(getAzimuth() + dAzimuth);
			setElevation(getElevation() + dElevation);

			if (getAzimuth() > M_PI) 
				mAzimuth -= 2.0*M_PI;
			if (getElevation() > M_PI) 
				mElevation -= 2.0*M_PI;

		//	fprintf(stderr, "az %f, elev %f\n", mAzimuth, mElevation);

			break;
		}
	case kActionZoom:
		{
			float dDolly = -mouseDelta[1] * kMouseZoomSensitivity;
			setDolly(getDolly() + dDolly);
			break;
		}
	case kActionTwist:
		// Not implemented
	default:
		break;
	}

}

void Camera::releaseMouse( int x, int y )
{
	mCurrentMouseAction = kActionNone;
}


void Camera::applyViewingTransform() {
	if( mDirtyTransform )
		calculateViewingTransformParameters();

	// Set Projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(mFOV,1.0,1.0,100.0);

	// Set View matrix			
	glMatrixMode(GL_MODELVIEW);
	// Place the camera at mPosition, aim the camera at
	// mLookAt, and twist the camera such that mUpVector is up
	gluLookAt(	mPosition[0], mPosition[1], mPosition[2],
				mLookAt[0],   mLookAt[1],   mLookAt[2],
				mUpVector[0], mUpVector[1], mUpVector[2]);

	Vec3f	F = mLookAt - mPosition;
	glRotatef( mTwist, F[0], F[1], F[2] );
}



#pragma warning(pop)
