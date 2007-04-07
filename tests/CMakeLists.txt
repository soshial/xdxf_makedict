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
ADD_TEST(t_dslpasrser "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_dslparser.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")