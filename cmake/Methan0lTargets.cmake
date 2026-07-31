function(methan0l_configure_cpp_target target)
	target_compile_features(${target} PUBLIC cxx_std_20)
	target_compile_definitions(${target} PUBLIC BITS=${METHAN0L_BITS})
	set_target_properties(${target} PROPERTIES
		CXX_EXTENSIONS OFF
		POSITION_INDEPENDENT_CODE ON
	)
	if(WIN32)
		set_property(TARGET ${target} PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
	endif()
endfunction()

function(methan0l_configure_core_library target output_name)
	methan0l_configure_cpp_target(${target})
	target_include_directories(${target} PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
	)
	set_target_properties(${target} PROPERTIES
		PREFIX "lib"
		OUTPUT_NAME "${output_name}"
		SUFFIX ".so"
		RUNTIME_OUTPUT_DIRECTORY "${METHAN0L_RUNTIME_DIR}"
		LIBRARY_OUTPUT_DIRECTORY "${METHAN0L_RUNTIME_DIR}"
		ARCHIVE_OUTPUT_DIRECTORY "${METHAN0L_ARCHIVE_DIR}"
		BUILD_RPATH "$ORIGIN"
		INSTALL_RPATH "$ORIGIN"
	)
endfunction()

function(methan0l_add_stdlib target output_name)
	add_library(${target} MODULE ${ARGN})
	methan0l_configure_cpp_target(${target})
	target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}")
	target_link_libraries(${target} PRIVATE methan0l)
	set_target_properties(${target} PROPERTIES
		PREFIX ""
		OUTPUT_NAME "${output_name}"
		SUFFIX ".so"
		RUNTIME_OUTPUT_DIRECTORY "${METHAN0L_RUNTIME_DIR}/libraries"
		LIBRARY_OUTPUT_DIRECTORY "${METHAN0L_RUNTIME_DIR}/libraries"
		ARCHIVE_OUTPUT_DIRECTORY "${METHAN0L_ARCHIVE_DIR}"
		BUILD_RPATH "$ORIGIN/.."
		INSTALL_RPATH "$ORIGIN/.."
	)
	add_dependencies(stdlib ${target})
	install(TARGETS ${target}
		RUNTIME DESTINATION libraries
		LIBRARY DESTINATION libraries
	)
endfunction()

function(methan0l_add_module target output_name)
	add_library(${target} MODULE ${ARGN})
	methan0l_configure_cpp_target(${target})
	target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}")
	target_link_libraries(${target} PRIVATE methan0l)
	set_target_properties(${target} PROPERTIES
		PREFIX ""
		OUTPUT_NAME "${output_name}"
		SUFFIX ".so"
		RUNTIME_OUTPUT_DIRECTORY "${METHAN0L_RUNTIME_DIR}/modules/${output_name}"
		LIBRARY_OUTPUT_DIRECTORY "${METHAN0L_RUNTIME_DIR}/modules/${output_name}"
		ARCHIVE_OUTPUT_DIRECTORY "${METHAN0L_ARCHIVE_DIR}"
		BUILD_RPATH "$ORIGIN/../.."
		INSTALL_RPATH "$ORIGIN/../.."
	)
	add_dependencies(modules ${target})
	install(TARGETS ${target}
		RUNTIME DESTINATION "modules/${output_name}"
		LIBRARY DESTINATION "modules/${output_name}"
	)
endfunction()
