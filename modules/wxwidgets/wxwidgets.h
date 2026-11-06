#ifndef MODULES_WXWIDGETS_WXWIDGETS_H_
#define MODULES_WXWIDGETS_WXWIDGETS_H_

#include <wx/app.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

// Keep wxWidgets first, then remove Win32 macros which collide with Methan0l's
// token and binding names.
#ifdef IN
#undef IN
#endif
#ifdef OUT
#undef OUT
#endif
#ifdef DELETE
#undef DELETE
#endif
#ifdef INTERFACE
#undef INTERFACE
#endif
#ifdef CONST
#undef CONST
#endif
#ifdef VOID
#undef VOID
#endif
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#ifdef THIS
#undef THIS
#endif

#include <methan0l/methan0l.h>

#include <memory>
#include <string>

namespace wxwidgets
{

template<typename T>
struct Handle
{
	T *value = nullptr;
};

class Window
{
	private:
		std::shared_ptr<Handle<wxFrame>> frame;
		wxBoxSizer *sizer = nullptr;

	public:
		Window(const std::string &title, int width, int height);
		~Window();

		wxFrame& native();
		void add(wxWindow *control, int proportion = 0);
		void show();
		void close();
};

class Label
{
	private:
		std::shared_ptr<Handle<wxStaticText>> label;

	public:
		Label(Window &parent, const std::string &text);
		std::string get_text() const;
		void set_text(const std::string &text);
};

class InputField
{
	private:
		std::shared_ptr<Handle<wxTextCtrl>> input;

	public:
		InputField(Window &parent, const std::string &value);
		std::string get_text() const;
		void set_text(const std::string &text);
};

class Button
{
	private:
		struct Callback
		{
			mtl::Value callable;
			mtl::Interpreter *context = nullptr;
		};

		std::shared_ptr<Handle<wxButton>> button;
		std::shared_ptr<Callback> callback;

	public:
		Button(Window &parent, const std::string &text);
		std::string get_text() const;
		void set_text(const std::string &text);
		void set_on_click(mtl::Interpreter &context, const mtl::Value &callable);
};

}

NATIVE_CLASS(Window, wxwidgets::Window)
NATIVE_CLASS(Button, wxwidgets::Button)
NATIVE_CLASS(Label, wxwidgets::Label)
NATIVE_CLASS(InputField, wxwidgets::InputField)

#endif
