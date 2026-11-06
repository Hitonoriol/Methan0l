#include "wxwidgets.h"
#include "CommonControlsActivationContext.h"

#include <methan0l/CoreLibrary.h>

#include <wx/init.h>
#include <wx/log.h>

#include <stdexcept>

class Methan0lWxApp: public wxApp
{
	public:
		bool OnInit() override
		{
			return true;
		}
};

wxIMPLEMENT_APP_NO_MAIN(Methan0lWxApp);

namespace
{

class WxRuntime
{
	private:
		bool initialized = false;

	public:
		WxRuntime()
		{
			CommonControlsActivationContext common_controls;
			int argc = 0;
			char **argv = nullptr;
			initialized = wxEntryStart(argc, argv);
			if (!initialized || wxTheApp == nullptr || !wxTheApp->CallOnInit()) {
				if (initialized)
					wxEntryCleanup();
				throw std::runtime_error("Could not initialize wxWidgets");
			}
		}

		~WxRuntime()
		{
			CommonControlsActivationContext common_controls;
			if (initialized) {
				wxTheApp->OnExit();
				wxEntryCleanup();
			}
		}
};

void ensure_wxwidgets()
{
	static WxRuntime runtime;
}

std::string utf8(const wxString &value)
{
	return value.ToUTF8().data();
}

void button_on_click(mtl::Object &this_obj, mtl::Value callable)
{
	Button::native(this_obj).set_on_click(this_obj.context(), callable);
}

int run()
{
	ensure_wxwidgets();
	CommonControlsActivationContext common_controls;
	wxTheApp->SetExitOnFrameDelete(true);
	return wxTheApp->MainLoop();
}

}

namespace wxwidgets
{

Window::Window(const std::string &title, const int width, const int height)
	: frame(std::make_shared<Handle<wxFrame>>())
{
	ensure_wxwidgets();
	CommonControlsActivationContext common_controls;
	frame->value = new wxFrame(
		nullptr,
		wxID_ANY,
		wxString::FromUTF8(title.c_str()),
		wxDefaultPosition,
		wxSize(width, height)
	);
	sizer = new wxBoxSizer(wxVERTICAL);
	frame->value->SetSizer(sizer);

	const std::weak_ptr<Handle<wxFrame>> handle = frame;
	frame->value->Bind(wxEVT_DESTROY, [handle](wxWindowDestroyEvent &event) {
		if (const auto locked = handle.lock())
			locked->value = nullptr;
			event.Skip();
	});
}

Window::~Window()
{
	CommonControlsActivationContext common_controls;
	if (frame.use_count() == 1 && frame->value != nullptr) {
		frame->value->Destroy();
		frame->value = nullptr;
	}
}

wxFrame& Window::native()
{
	if (frame->value == nullptr)
		throw std::runtime_error("The window is closed");
	return *frame->value;
}

void Window::add(wxWindow *control, const int proportion)
{
	sizer->Add(control, proportion, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 8);
	native().Layout();
}

void Window::show()
{
	native().Show(true);
}

void Window::close()
{
	native().Close(true);
}

Label::Label(Window &parent, const std::string &text)
	: label(std::make_shared<Handle<wxStaticText>>())
{
	CommonControlsActivationContext common_controls;
	label->value = new wxStaticText(
		&parent.native(), wxID_ANY, wxString::FromUTF8(text.c_str())
	);
	parent.add(label->value);

	const std::weak_ptr<Handle<wxStaticText>> handle = label;
	label->value->Bind(wxEVT_DESTROY, [handle](wxWindowDestroyEvent &event) {
		if (const auto locked = handle.lock())
			locked->value = nullptr;
		event.Skip();
	});
}

std::string Label::get_text() const
{
	return label->value == nullptr ? std::string() : utf8(label->value->GetLabel());
}

void Label::set_text(const std::string &text)
{
	if (label->value != nullptr)
		label->value->SetLabel(wxString::FromUTF8(text.c_str()));
}

InputField::InputField(Window &parent, const std::string &value)
	: input(std::make_shared<Handle<wxTextCtrl>>())
{
	CommonControlsActivationContext common_controls;
	input->value = new wxTextCtrl(
		&parent.native(), wxID_ANY, wxString::FromUTF8(value.c_str())
	);
	parent.add(input->value);

	const std::weak_ptr<Handle<wxTextCtrl>> handle = input;
	input->value->Bind(wxEVT_DESTROY, [handle](wxWindowDestroyEvent &event) {
		if (const auto locked = handle.lock())
			locked->value = nullptr;
		event.Skip();
	});
}

std::string InputField::get_text() const
{
	return input->value == nullptr ? std::string() : utf8(input->value->GetValue());
}

void InputField::set_text(const std::string &text)
{
	if (input->value != nullptr)
		input->value->SetValue(wxString::FromUTF8(text.c_str()));
}

Button::Button(Window &parent, const std::string &text)
	: button(std::make_shared<Handle<wxButton>>()),
	  callback(std::make_shared<Callback>())
{
	CommonControlsActivationContext common_controls;
	button->value = new wxButton(
		&parent.native(), wxID_ANY, wxString::FromUTF8(text.c_str())
	);
	parent.add(button->value);

	const std::weak_ptr<Handle<wxButton>> handle = button;
	button->value->Bind(wxEVT_DESTROY, [handle](wxWindowDestroyEvent &event) {
		if (const auto locked = handle.lock())
			locked->value = nullptr;
		event.Skip();
	});

	const std::weak_ptr<Callback> callback_state = callback;
	button->value->Bind(wxEVT_BUTTON, [callback_state](wxCommandEvent&) {
		const auto state = callback_state.lock();
		if (state == nullptr || state->context == nullptr || !state->callable.is_callable())
			return;

		try {
			mtl::ExprList args;
			state->context->invoke(state->callable, args);
		} catch (const std::exception &error) {
			wxLogError("Methan0l callback failed: %s", error.what());
		} catch (...) {
			wxLogError("Methan0l callback failed");
		}
	});
}

std::string Button::get_text() const
{
	return button->value == nullptr ? std::string() : utf8(button->value->GetLabel());
}

void Button::set_text(const std::string &text)
{
	if (button->value != nullptr)
		button->value->SetLabel(wxString::FromUTF8(text.c_str()));
}

void Button::set_on_click(mtl::Interpreter &context, const mtl::Value &callable)
{
	if (!callable.is_callable())
		throw std::runtime_error("Button.on_click expects a callable");
	callback->context = &context;
	callback->callable = callable;
}

}

LOAD_MODULE
{
	ensure_wxwidgets();
	native_class<Window>();
	native_class<Button>();
	native_class<Label>();
	native_class<InputField>();
	function("run", run);
}

using mtl::Methods;

NATIVE_CLASS_BINDING(Window, {
	BIND_CONSTRUCTOR(const std::string&, int, int)
	BIND_METHOD(show)
	BIND_METHOD(close)
})

NATIVE_CLASS_BINDING(Button, {
	BIND_CONSTRUCTOR(wxwidgets::Window&, const std::string&)
	BIND_METHOD(get_text)
	BIND_METHOD(set_text)
	BIND_EXTERNAL_METHOD_AS("on_click", button_on_click)
})

NATIVE_CLASS_BINDING(Label, {
	BIND_CONSTRUCTOR(wxwidgets::Window&, const std::string&)
	BIND_METHOD(get_text)
	BIND_METHOD(set_text)
})

NATIVE_CLASS_BINDING(InputField, {
	BIND_CONSTRUCTOR(wxwidgets::Window&, const std::string&)
	BIND_METHOD(get_text)
	BIND_METHOD(set_text)
})
