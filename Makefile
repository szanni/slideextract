.PHONY: all install dist clean distclean

PACKAGE = slideextract
VERSION = 0.1.0
DIST = Makefile ${PACKAGE}.h ${PACKAGE}.cpp ${PACKAGE}.1 main.c LICENSE CHANGELOG README.md

PREFIX ?= /usr/local
CC ?= gcc
CXX ?= g++

CPPFLAGS += -D'VERSION="${VERSION}"'
CPPFLAGS += -Wall -Wextra -pedantic
CPPFLAGS += `pkg-config --cflags 'opencv4 >= 4.0'`
#LDFLAGS += `pkg-config --libs 'opencv4 >= 4.0'`
# Can not use pkg-config --libs here as it pulls in all optional dependencies, too.
LDFLAGS += -lopencv_highgui -lopencv_videoio -lopencv_video -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

all: ${PACKAGE}

${PACKAGE}: ${PACKAGE}.o main.o
	${CXX} ${PACKAGE}.o main.o -o $@ ${CXXFLAGS} ${LDFLAGS}

install: ${PACKAGE} ${PACKAGE}.1
	install -D -m755 ${PACKAGE} ${DESTDIR}${PREFIX}/bin/${PACKAGE}
	install -D -m644 ${PACKAGE}.1 ${DESTDIR}${PREFIX}/share/man/man1/${PACKAGE}.1

dist: ${DIST}
	mkdir -p ${PACKAGE}-${VERSION}
	cp ${DIST} ${PACKAGE}-${VERSION}
	tar -cf ${PACKAGE}-${VERSION}.tar ${PACKAGE}-${VERSION}
	gzip -fk ${PACKAGE}-${VERSION}.tar
	xz -f ${PACKAGE}-${VERSION}.tar

clean:
	rm -f ${PACKAGE} ${PACKAGE}.o main.o

distclean:
	rm -rf ${PACKAGE}-${VERSION}{,.tar,.tar.gz,.tar.xz}
