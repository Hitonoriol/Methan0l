file(GLOB_RECURSE CH3OH_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE CH3OH_HEADERS CONFIGURE_DEPENDS "*.h")

# The legacy build compiled these common implementations into both core
# libraries. Keep that ABI behavior while giving the sources one owner.
file(GLOB METHAN0L_UTILITY_SOURCES CONFIGURE_DEPENDS
	"${PROJECT_SOURCE_DIR}/methan0l/util/*.cpp"
)

add_library(ch3oh SHARED
	${CH3OH_SOURCES}
	${CH3OH_HEADERS}
	${METHAN0L_UTILITY_SOURCES}
)
methan0l_configure_core_library(ch3oh ch3oh)
target_link_libraries(ch3oh PUBLIC Boost::container)

install(TARGETS ch3oh
	RUNTIME DESTINATION .
	LIBRARY DESTINATION .
	ARCHIVE DESTINATION .
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
	DESTINATION "include/ch3oh"
	FILES_MATCHING PATTERN "*.h"
)
