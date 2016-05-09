#pragma warning(disable : 4786)

#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <string>
#include <FL/fl_ask.H>

#include "modelerui.h"
#include "camera.h"

using namespace std;

inline void ModelerUI::cb_hide_i(Fl_Window*, void*) 
{
	m_pwndModelerWnd->hide();
}

void ModelerUI::cb_hide(Fl_Window* o, void* v)
{
	((ModelerUI*)(o->user_data()))->cb_hide_i(o,v);
}

/******************/

ModelerUI::ModelerUI() 
{
	m_pwndModelerWnd->callback((Fl_Callback*)cb_hide);
	m_pwndModelerWnd->when(FL_HIDE);
}

void ModelerUI::show() 
{
	// m_pwndModelerWnd->resize(40 + m_pwndMainWnd->w(), 30,
	// 	m_pwndModelerWnd->w(), m_pwndModelerWnd->h());
	m_pwndModelerWnd->show();
	m_pwndScoreWnd->show();
}

void ModelerUI::replaceModelerView(ModelerView* pwndNewModelerView)
{
	m_pwndModelerWnd->remove(*m_pwndModelerView);
	delete m_pwndModelerView;

	m_pwndModelerView = pwndNewModelerView;
	m_pwndModelerView->resize(0, 10, m_pwndModelerWnd->w(), m_pwndModelerWnd->h()/1);
	m_pwndModelerWnd->add_resizable(*m_pwndModelerView);
}

