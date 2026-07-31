file(GLOB_RECURSE MODULE_NCURSES_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE MODULE_NCURSES_HEADERS CONFIGURE_DEPENDS "*.h")

methan0l_add_module(module_ncurses ncurses
	${MODULE_NCURSES_SOURCES}
	${MODULE_NCURSES_HEADERS}
)

if(METHAN0L_CURSES_TARGET)
	target_link_libraries(module_ncurses PRIVATE ${METHAN0L_CURSES_TARGET})
else()
	target_include_directories(module_ncurses PRIVATE ${CURSES_INCLUDE_DIRS})
	target_compile_options(module_ncurses PRIVATE ${CURSES_CFLAGS})
	target_link_libraries(module_ncurses PRIVATE ${CURSES_LIBRARIES})
endif()
