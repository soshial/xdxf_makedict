PROJECT(makedict_tests)

SET(MAKEDICT_ROOT "..")

INCLUDE_DIRECTORIES(${MAKEDICT_ROOT}/src)

#tools for testing

ADD_EXECUTABLE(validate_stardict
	validate_stardict.cpp)
TARGET_LINK_LIBRARIES(validate_stardict
	${GLIB2_LIBRARIES})

REMOVE_DEFINITIONS(-DDUMMY_FORMAT_NAME="\\\"dummy\\\"")
ADD_DEFINITIONS(
	-DDUMMY_FORMAT_NAME="\\\"single-dummy\\\""
	-DDUMMY_PARSER_ALONE
	-DDUMMY_GENERATOR_ALONE)

ADD_EXECUTABLE(dummy_parser
	${MAKEDICT_ROOT}/src/dummy_parser.cpp
	${MAKEDICT_ROOT}/src/parser.cpp ${MAKEDICT_ROOT}/src/parser.hpp
	${MAKEDICT_ROOT}/src/utils.cpp ${MAKEDICT_ROOT}/src/utils.hpp
	${MAKEDICT_ROOT}/src/file.cpp ${MAKEDICT_ROOT}/src/file.hpp
	${MAKEDICT_ROOT}/src/xml.cpp ${MAKEDICT_ROOT}/src/xml.hpp
	${MAKEDICT_ROOT}/src/log.cpp ${MAKEDICT_ROOT}/src/log.hpp
)
TARGET_LINK_LIBRARIES(dummy_parser
	${GLIB2_LIBRARIES} ${EXPAT_LIBRARIES})

ADD_EXECUTABLE(dummy_generator
	${MAKEDICT_ROOT}/src/dummy_generator.cpp
	${MAKEDICT_ROOT}/src/generator.cpp ${MAKEDICT_ROOT}/src/generator.hpp
	${MAKEDICT_ROOT}/src/utils.cpp ${MAKEDICT_ROOT}/src/utils.hpp
	${MAKEDICT_ROOT}/src/file.cpp ${MAKEDICT_ROOT}/src/file.hpp
	${MAKEDICT_ROOT}/src/xml.cpp ${MAKEDICT_ROOT}/src/xml.hpp
	${MAKEDICT_ROOT}/src/log.cpp ${MAKEDICT_ROOT}/src/log.hpp
)
TARGET_LINK_LIBRARIES(dummy_generator
	${GLIB2_LIBRARIES} ${EXPAT_LIBRARIES})
#end of tools for testing

ADD_EXECUTABLE(t_normalize
	t_normalize.cpp
	${MAKEDICT_ROOT}/src/normalize_tags.cpp
	${MAKEDICT_ROOT}/src/normalize_tags.hpp
)
TARGET_LINK_LIBRARIES(t_normalize
	${GLIB2_LIBRARIES})

ADD_TEST(t_normalize ${EXECUTABLE_OUPUT_PATH}/t_normalize)

ADD_TEST(t_dslparser "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_dslparser.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/..")

ADD_EXECUTABLE(t_xml
        t_xml.cpp 
        ${MAKEDICT_ROOT}/src/xml.cpp ${MAKEDICT_ROOT}/src/xml.hpp
        ${MAKEDICT_ROOT}/src/file.cpp ${MAKEDICT_ROOT}/src/file.hpp)
TARGET_LINK_LIBRARIES(t_xml
	${GLIB2_LIBRARIES} ${EXPAT_LIBRARIES})
ADD_TEST(t_xml ${EXECUTABLE_OUPUT_PATH}/t_xml)

ADD_TEST(t_stardict_parser "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_stardict_parser.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_xdxfgenerator "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_xdxfgenerator.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_stardict "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_stardict.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_dummy "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_dummy.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_generator "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_generator.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_parser_error "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_parser_error.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_parser_options "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_parser_options.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_sdict_parser "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_sdict_parser.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_generator_error "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_generator_error.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(t_perfomance "/bin/sh" "${CMAKE_CURRENT_SOURCE_DIR}/t_perfomance.sh"
	"${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

