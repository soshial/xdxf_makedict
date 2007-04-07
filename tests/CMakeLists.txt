PROJECT(makedict_tests)

SET(MAKEDICT_ROOT "..")

INCLUDE_DIRECTORIES(${MAKEDICT_ROOT}/src)

ADD_EXECUTABLE(t_normalize
	t_normalize.cpp
	${MAKEDICT_ROOT}/src/normalize_tags.cpp
	${MAKEDICT_ROOT}/src/normalize_tags.hpp
)
TARGET_LINK_LIBRARIES(t_normalize
	${GLIB2_LIBRARIES})

ADD_TEST(t_normalize ${EXECUTABLE_OUPUT_PATH}/t_normalize)

ADD_TEST(t_dslparser "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_dslparser.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_EXECUTABLE(t_xml
        t_xml.cpp 
        ${MAKEDICT_ROOT}/src/xml.cpp ${MAKEDICT_ROOT}/src/xml.hpp
        ${MAKEDICT_ROOT}/src/file.cpp ${MAKEDICT_ROOT}/src/file.hpp)
TARGET_LINK_LIBRARIES(t_xml
	${GLIB2_LIBRARIES} ${EXPAT_LIBRARIES})
ADD_TEST(t_xml ${EXECUTABLE_OUPUT_PATH}/t_xml)

ADD_TEST(t_stardict_parser "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_stardict_parser.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")