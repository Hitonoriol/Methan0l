file(GLOB_RECURSE STDLIB_CLI_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE STDLIB_CLI_HEADERS CONFIGURE_DEPENDS "*.h")

methan0l_add_stdlib(stdlib_cli LibCLI
	${STDLIB_CLI_SOURCES}
	${STDLIB_CLI_HEADERS}
	${PROJECT_SOURCE_DIR}/methan0l/lang/util/hash.cpp
	${PROJECT_SOURCE_DIR}/cli/Methan0l.cpp
	${PROJECT_SOURCE_DIR}/cli/Runner.cpp
	${PROJECT_SOURCE_DIR}/cli/InteractiveRunner.cpp
	${PROJECT_SOURCE_DIR}/cli/Baker.cpp
)
