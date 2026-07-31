file(GLOB_RECURSE METHAN0L_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE METHAN0L_HEADERS CONFIGURE_DEPENDS "*.h")

add_library(methan0l SHARED ${METHAN0L_SOURCES} ${METHAN0L_HEADERS})
methan0l_configure_core_library(methan0l methan0l)
target_link_libraries(methan0l
	PUBLIC
		ch3oh
		Boost::dll
		Boost::filesystem
		Boost::system
	PRIVATE
		${CMAKE_DL_LIBS}
)

install(TARGETS methan0l
	RUNTIME DESTINATION .
	LIBRARY DESTINATION .
	ARCHIVE DESTINATION .
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
	DESTINATION "include/methan0l"
	FILES_MATCHING PATTERN "*.h"
)
