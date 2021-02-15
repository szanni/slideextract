cmake_minimum_required(VERSION 3.10)

set(VERSION "0.1.0")

project(
   slideextract_project
   VERSION ${VERSION}
)

# Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# main.c
add_compile_definitions(VERSION=${VERSION})
add_executable(slideextract main.c slideextract.cpp slideextract.h)
target_link_libraries(slideextract PRIVATE ${CONAN_LIBS})
