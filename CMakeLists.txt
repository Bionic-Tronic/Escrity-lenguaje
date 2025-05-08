cmake_minimum_required(VERSION 3.31.3)
project(escrity)
include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
)
file(GLOB SRCS
	"${PROJECT_SOURCE_DIR}/include/*"
	"${PROJECT_SOURCE_DIR}/src/*"
)
set(SOURCES
    src/main.c
    src/interpret.c
    src/evaluator.c
    src/tokenizer.c
    src/protocol.c
    src/errors.c
    src/function.c
    src/variable.c
    src/object.c
    src/properties.c
    src/array.c
    src/enums.c
)
add_executable(escrity ${SRCS})
find_package(SQLite3 REQUIRED)
target_link_libraries(escrity ${SQLite3_LIBRARIES})
find_package(Curses REQUIRED)
target_link_libraries(escrity ${CURSES_LIBRARIES})
find_package(CURL REQUIRED)
target_link_libraries(escrity ${CURL_LIBRARIES})
