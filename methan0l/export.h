#ifndef METHAN0L_EXPORT_H_
#define METHAN0L_EXPORT_H_

#if defined(_WIN32)
#	if defined(methan0l_EXPORTS) || defined(ch3oh_EXPORTS)
#		define METHAN0L_API __declspec(dllexport)
#	else
#		define METHAN0L_API __declspec(dllimport)
#	endif
#else
#	define METHAN0L_API
#endif

#endif /* METHAN0L_EXPORT_H_ */
