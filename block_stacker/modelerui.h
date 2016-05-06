#ifndef modelerui_h
#define modelerui_h

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_message.H>

#include "modelerview.h"
#include "modelerdraw.h"
#include "modelerapp.h"
#include "animatoruiwindows.h"

class ModelerUI : public ModelerUIWindows
{
public:
	typedef void (ValueChangedCallback)();

	ModelerUI();
	void show();
	void replaceModelerView(ModelerView* pwndNewModelerView);


private:

	inline void cb_hide_i(Fl_Window*, void*);
	static void cb_hide(Fl_Window*, void*);
	inline void cb_normal_i(Fl_Menu_*, void*);
};

#endif

