add_custom_target(stdlib ALL)

set(STDLIB_SCRIPT_OUTPUT "${METHAN0L_RUNTIME_DIR}/libraries/Mutable.mt0")
add_custom_command(
	OUTPUT "${STDLIB_SCRIPT_OUTPUT}"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${METHAN0L_RUNTIME_DIR}/libraries"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/Mutable.mt0"
		"${STDLIB_SCRIPT_OUTPUT}"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Mutable.mt0"
	VERBATIM
)
add_custom_target(stdlib_scripts DEPENDS "${STDLIB_SCRIPT_OUTPUT}")
add_dependencies(stdlib stdlib_scripts)

add_subdirectory(arithmetic)
add_subdirectory(cli)
add_subdirectory(container)
add_subdirectory(data)
add_subdirectory(internal)
add_subdirectory(io)
add_subdirectory(logical)
add_subdirectory(math)
add_subdirectory(module)
add_subdirectory(string)
add_subdirectory(util)

install(FILES Mutable.mt0 DESTINATION libraries)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
	DESTINATION "include/stdlib"
	FILES_MATCHING PATTERN "*.h"
)
