cmake_minimum_required(VERSION 3.21)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	project(Methan0lWxWidgetsModule LANGUAGES CXX)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/Methan0lModule.cmake")
methan0l_bootstrap_module()

find_package(wxWidgets CONFIG REQUIRED)

methan0l_add_module(module_wxwidgets wxwidgets
	CommonControlsActivationContext.h
	wxwidgets.cpp
	wxwidgets.h
)

if(WIN32)
	target_sources(module_wxwidgets PRIVATE CommonControlsActivationContext.cpp)
endif()

target_link_libraries(module_wxwidgets PRIVATE wx::base wx::core)
