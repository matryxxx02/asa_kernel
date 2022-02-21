# $Id: Makefile 167 2015-01-23 17:51:37Z phm $
##############################################################################
ROOTDIR=../
LIBNAME=hardware

RM	= rm
CC	= gcc
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = $(ROOTDIR)/include
CFLAGS	= -Wall -ansi -pedantic -I$(INCDIR)
LDLIBS	= -L$(LIBDIR) -l$(LIBNAME)

all: TP3

TP3.o: TP3.c

TP3: TP3.o

clean:
	$(RM) -f *.o TP3 vdisk?.bin

.PHONY: clean
