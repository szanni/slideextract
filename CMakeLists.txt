cmake_minimum_required(VERSION 3.10)

set(VERSION "0.1.0")

project(
   slideextract_project
   VERSION ${VERSION}
   LANGUAGES CXX
)

# Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


# slideextract.cpp
add_library(slideextract STATIC slideextract.cpp slideextract.h)
target_link_libraries(slideextract PRIVATE ${CONAN_LIBS})


# main.c
add_compile_definitions(VERSION=${VERSION})
add_executable(main main.c)
target_link_libraries(main PRIVATE slideextract ${CONAN_LIBS})
