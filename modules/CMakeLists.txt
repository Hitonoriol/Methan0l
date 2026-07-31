add_custom_target(modules ALL)

add_subdirectory(system)
add_subdirectory(test)

if(WIN32)
	find_package(unofficial-pdcurses CONFIG REQUIRED)
	set(METHAN0L_CURSES_TARGET unofficial::pdcurses::pdcurses)
else()
	# vcpkg's ncurses port installs the wide-character ncursesw variant.
	# Tell CMake's FindCurses module to search for that library and headers.
	set(CURSES_NEED_WIDE TRUE)
	find_package(Curses REQUIRED)
endif()

add_subdirectory(ncurses)
