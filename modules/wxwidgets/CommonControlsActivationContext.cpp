#include "CommonControlsActivationContext.h"

#ifdef _WIN32

#include <windows.h>

#include <array>
#include <stdexcept>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace
{

HANDLE common_controls_handle()
{
	static const HANDLE handle = [] {
		const auto module = reinterpret_cast<HMODULE>(&__ImageBase);
		std::array<wchar_t, 32768> module_path{};
		if (GetModuleFileNameW(module, module_path.data(), module_path.size()) == 0)
			throw std::runtime_error("Could not locate the wxwidgets module");

		ACTCTXW context{};
		context.cbSize = sizeof(context);
		context.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
		context.lpSource = module_path.data();
		context.lpResourceName = MAKEINTRESOURCEW(2);

		const HANDLE created = CreateActCtxW(&context);
		if (created == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Could not load the wxwidgets manifest");
		return created;
	}();
	return handle;
}

}

CommonControlsActivationContext::CommonControlsActivationContext()
{
	ULONG_PTR activation_cookie = 0;
	if (!ActivateActCtx(common_controls_handle(), &activation_cookie))
		throw std::runtime_error("Could not activate the wxwidgets manifest");
	cookie = activation_cookie;
}

CommonControlsActivationContext::~CommonControlsActivationContext()
{
	if (cookie != 0)
		DeactivateActCtx(0, static_cast<ULONG_PTR>(cookie));
}

#endif
