bin_PROGRAMS = slideextract
slideextract_SOURCES = src/main.cpp src/slideextract.h src/slideextract.cpp
slideextract_LDADD = $(OPENCV_LIBS)
AM_CFLAGS = -I/usr/include/opencv4 -std=c99 -Wall -Wextra -pedantic -D_XOPEN_SOURCE=300 $(OPENCV_CFLAGS)
AM_CXXFLAGS = -std=c++11 -I/usr/include/opencv4

dist_man_MANS = man/slideextract.1

