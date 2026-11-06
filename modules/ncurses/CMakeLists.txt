cmake_minimum_required(VERSION 3.21)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	project(Methan0lNcursesModule LANGUAGES CXX)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/Methan0lModule.cmake")
methan0l_bootstrap_module()

if(WIN32)
	find_package(unofficial-pdcurses CONFIG REQUIRED)
	set(METHAN0L_CURSES_TARGET unofficial::pdcurses::pdcurses)
else()
	set(CURSES_NEED_WIDE TRUE)
	find_package(Curses REQUIRED)
endif()

file(GLOB_RECURSE MODULE_NCURSES_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE MODULE_NCURSES_HEADERS CONFIGURE_DEPENDS "*.h")

methan0l_add_module(module_ncurses ncurses
	${MODULE_NCURSES_SOURCES}
	${MODULE_NCURSES_HEADERS}
)

if(METHAN0L_CURSES_TARGET)
	target_link_libraries(module_ncurses PRIVATE ${METHAN0L_CURSES_TARGET})
	if(WIN32)
		get_target_property(METHAN0L_CURSES_LIBRARY
			${METHAN0L_CURSES_TARGET} IMPORTED_LOCATION_RELEASE)
		get_filename_component(METHAN0L_CURSES_ROOT
			"${METHAN0L_CURSES_LIBRARY}/../.." ABSOLUTE)
		find_file(METHAN0L_CURSES_RUNTIME
			NAMES pdcurses.dll curses.dll
			HINTS "${METHAN0L_CURSES_ROOT}/bin"
			NO_DEFAULT_PATH)
		if(METHAN0L_CURSES_RUNTIME)
			add_custom_command(TARGET module_ncurses POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"${METHAN0L_CURSES_RUNTIME}"
					"${METHAN0L_RUNTIME_DIR}"
			)
			install(FILES "${METHAN0L_CURSES_RUNTIME}" DESTINATION .)
		endif()
	endif()
else()
	target_include_directories(module_ncurses PRIVATE ${CURSES_INCLUDE_DIRS})
	target_compile_options(module_ncurses PRIVATE ${CURSES_CFLAGS})
	target_link_libraries(module_ncurses PRIVATE ${CURSES_LIBRARIES})
endif()
