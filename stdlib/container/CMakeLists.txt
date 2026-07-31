file(GLOB_RECURSE STDLIB_CONTAINER_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE STDLIB_CONTAINER_HEADERS CONFIGURE_DEPENDS "*.h")

methan0l_add_stdlib(stdlib_container LibContainer
	${STDLIB_CONTAINER_SOURCES}
	${STDLIB_CONTAINER_HEADERS}
	${PROJECT_SOURCE_DIR}/methan0l/lang/util/hash.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Iterable.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Collection.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/List.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Set.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/DefaultMapIterator.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/AbstractMap.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Map.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Iterator.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Range.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Mapping.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/Filter.cpp
	${PROJECT_SOURCE_DIR}/methan0l/core/class/String.cpp
)
