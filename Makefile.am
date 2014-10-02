bin_PROGRAMS = slideextract
slideextract_SOURCES = src/main.c src/slideextract.h src/slideextract.c
slideextract_LDADD = $(OPENCV_LIBS)
AM_CFLAGS = -std=c99 -Wall -Wextra -pedantic -D_XOPEN_SOURCE=300 $(OPENCV_CFLAGS)

dist_man_MANS = man/slideextract.1

