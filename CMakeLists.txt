cmake_minimum_required(VERSION 3.21)

option(METHAN0L_BUILD_MODULES "Build the native Methan0l modules" ON)

# The vcpkg toolchain is evaluated by project(), so select the dependencies for
# the complete module set before then.
if(METHAN0L_BUILD_MODULES)
	list(APPEND VCPKG_MANIFEST_FEATURES modules)
	list(REMOVE_DUPLICATES VCPKG_MANIFEST_FEATURES)
endif()

project(Methan0l VERSION 1.0.0 LANGUAGES CXX)

set(METHAN0L_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
math(EXPR METHAN0L_BITS "${CMAKE_SIZEOF_VOID_P} * 8")

if(POLICY CMP0167)
	cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost CONFIG REQUIRED COMPONENTS container dll filesystem system)

set(METHAN0L_RUNTIME_DIR "${CMAKE_BINARY_DIR}/runtime/$<CONFIG>")
set(METHAN0L_ARCHIVE_DIR "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Methan0lTargets)

add_subdirectory(ch3oh)
add_subdirectory(methan0l)
add_subdirectory(stdlib)
add_subdirectory(cli)

if(METHAN0L_BUILD_MODULES)
	add_subdirectory(modules)
endif()

install(DIRECTORY examples baker DESTINATION .)

set(CPACK_PACKAGE_NAME "Methan0l")
set(CPACK_PACKAGE_FILE_NAME "Methan0l-${CMAKE_SYSTEM_NAME}_${METHAN0L_BITS}")
set(CPACK_GENERATOR TGZ)
include(CPack)
