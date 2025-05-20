cmake_minimum_required(VERSION 3.20.0)
project(escrity)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)
set(SOURCES
    src/main.c
    src/interpret.c
    src/tokenizer.c
    src/evaluator.c
    src/error.c
    src/variable.c
    src/str.c
    src/array.c
    src/enum.c
    src/function.c
    src/object.c
    src/properties.c
    src/protocol.c
)
add_executable(escrity ${SOURCES})
