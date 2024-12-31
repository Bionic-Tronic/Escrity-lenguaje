cmake_minimum_required(VERSION 3.31.3)
project(Escrity)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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
)
add_executable(Escrity ${SRCS})
