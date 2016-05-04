// modelerview.h

// This is the base class for all your models.  It contains
// a camera control for your use.  The draw() function will 
// set up default lighting and apply the projection, so if you 
// inherit, you will probably want to call ModelerView::draw()
// to set up the camera.

#ifndef MODELERVIEW_H
#define MODELERVIEW_H

#include <FL/Fl_Gl_Window.H>

class Camera;
class ModelerView;
typedef ModelerView* (*ModelerViewCreator_f)(int x, int y, int w, int h, char *label);

typedef enum { CTRL_MODE, CURVE_MODE } cam_mode_t;

class Parameter{//zyc
public:
	Parameter();
	~Parameter();
	float theta_model;
	void set_theta(float val);
	float get_theta();


	// float phi = 55.0;
	// float psi = 30.0;
	// float cr = 0.0;
	// float h1 = 0.8;
	// float h2 = 3.0;
	// float h3 = 2.5;
	// float pc = 5.0;

};

	//Parameter *para;//zyc


class ModelerView : public Fl_Gl_Window
{
public:
    ModelerView(int x, int y, int w, int h, char *label=0);

	virtual ~ModelerView();
    virtual int handle(int event);
    virtual void draw();

	void setBMP(const char *fname);
	void saveBMP(const char* szFileName);
	void endDraw();

	void camera(cam_mode_t mode);
    Camera *m_camera;
	Camera *m_ctrl_camera;
	Camera *m_curve_camera;
	//Parameter *para;//zyc

	float t;
	void update();
	bool save_bmp;
};


#endif
