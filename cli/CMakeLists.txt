file(GLOB_RECURSE METHAN0L_CLI_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE METHAN0L_CLI_HEADERS CONFIGURE_DEPENDS "*.h")

add_executable(methan0l_cli
	${METHAN0L_CLI_SOURCES}
	${METHAN0L_CLI_HEADERS}
)
methan0l_configure_cpp_target(methan0l_cli)
target_include_directories(methan0l_cli PRIVATE "${PROJECT_SOURCE_DIR}")
target_link_libraries(methan0l_cli PRIVATE methan0l)
set_target_properties(methan0l_cli PROPERTIES
	OUTPUT_NAME methan0l
	RUNTIME_OUTPUT_DIRECTORY "${METHAN0L_RUNTIME_DIR}"
	BUILD_RPATH "$ORIGIN"
	INSTALL_RPATH "$ORIGIN"
)

install(TARGETS methan0l_cli
	RUNTIME DESTINATION .
)

if(WIN32)
	set(METHAN0L_BOOST_RUNTIME_TARGETS)
	foreach(boost_target Boost::container Boost::filesystem)
		get_target_property(boost_target_type ${boost_target} TYPE)
		if(boost_target_type STREQUAL "SHARED_LIBRARY"
			OR boost_target_type STREQUAL "MODULE_LIBRARY"
			OR boost_target_type STREQUAL "UNKNOWN_LIBRARY"
		)
			list(APPEND METHAN0L_BOOST_RUNTIME_TARGETS ${boost_target})
		endif()
	endforeach()
	if(METHAN0L_BOOST_RUNTIME_TARGETS)
		install(IMPORTED_RUNTIME_ARTIFACTS
			${METHAN0L_BOOST_RUNTIME_TARGETS}
			RUNTIME DESTINATION .
		)
	endif()
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
	DESTINATION "include/cli"
	FILES_MATCHING PATTERN "*.h"
)
