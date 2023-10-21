#include <methan0l.h>

#include <wx/wxprec.h>
 
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class App : public wxApp
{
	public:
		virtual bool OnInit();
};

NATIVE_CLASS(WxFrame, wxFrame)