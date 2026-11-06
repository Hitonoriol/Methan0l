#ifndef MODULES_WXWIDGETS_COMMONCONTROLSACTIVATIONCONTEXT_H_
#define MODULES_WXWIDGETS_COMMONCONTROLSACTIVATIONCONTEXT_H_

#include <cstdint>

class CommonControlsActivationContext
{
	public:
#ifdef _WIN32
		CommonControlsActivationContext();
		~CommonControlsActivationContext();
#else
		CommonControlsActivationContext() = default;
		~CommonControlsActivationContext() = default;
#endif

		CommonControlsActivationContext(const CommonControlsActivationContext&) = delete;
		CommonControlsActivationContext& operator=(const CommonControlsActivationContext&) = delete;

	private:
#ifdef _WIN32
		std::uintptr_t cookie = 0;
#endif
};

#endif /* MODULES_WXWIDGETS_COMMONCONTROLSACTIVATIONCONTEXT_H_ */
