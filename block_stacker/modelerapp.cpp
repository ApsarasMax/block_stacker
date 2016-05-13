#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerapp.h"
#include "modelerview.h"
#include "modelerui.h"
#include "camera.h"

#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Box.H>

#include <cstring>
#include <cstdio>
#include <cstdlib>


// ****************************************************************************


// Set the singleton initially to a NULL instance
ModelerApplication* ModelerApplication::m_instance = NULL;

// CLASS ModelerApplication METHODS

ModelerApplication* ModelerApplication::Instance()
{
	// Make a new instance if none exists, otherwise, return
	// the existing instance of the ModelerApplication
	return (m_instance) ? (m_instance) : (m_instance = new ModelerApplication());
}

void ModelerApplication::Init(ModelerViewCreator_f createView)
{


#ifdef WIN32
	DWORD dwBtnFaceColor = GetSysColor(COLOR_BTNFACE);

	// Get consistent background color
	Fl::background(GetRValue(dwBtnFaceColor), 
		GetGValue(dwBtnFaceColor),
		GetBValue(dwBtnFaceColor));
#endif

    // ********************************************************
    // Create the FLTK user interface
    // ********************************************************
    
    m_ui = new ModelerUI();
    
	modelerView = createView(0, 0, 100, 100 ,NULL);
	m_ui->replaceModelerView(modelerView);
}

ModelerApplication::~ModelerApplication()
{
    // FLTK handles widget deletion
    delete m_ui;
}

int ModelerApplication::Run()
{

    // Just tell FLTK to go for it.
   	Fl::visual( FL_RGB | FL_DOUBLE );
	m_ui->show();
	Fl::add_timeout(0, ModelerApplication::RedrawLoop, NULL);

	return Fl::run();
}

ParticleSystem *ModelerApplication::GetParticleSystem()
{
	return ps;
}

void ModelerApplication::SetParticleSystem(ParticleSystem *s)
{
	ps = s;
}

void ModelerApplication::RedrawLoop(void*)
{
	// 1/50 second update is good enough
	Fl::add_timeout(0.025, ModelerApplication::RedrawLoop, NULL);
}
