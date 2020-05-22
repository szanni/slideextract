bin_PROGRAMS = slideextract
slideextract_SOURCES = src/main.c src/slideextract.h src/slideextract.cpp
slideextract_LDADD = $(OPENCV_LIBS)
AM_CFLAGS = -std=c99 -Wall -Wextra -pedantic -D_XOPEN_SOURCE=300 $(OPENCV_CFLAGS)
AM_CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -D_XOPEN_SOURCE=300 $(OPENCV_CFLAGS) -I/usr/include/opencv4 -lopencv_highgui -lopencv_videoio -lopencv_video -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

dist_man_MANS = man/slideextract.1

