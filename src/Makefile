INCLUDE_DIR=includes
LIB_DIR=lib
HEADER_FILE=includes/drive.c includes/init.c includes/volume.c includes/superbloc.c 

WARNING_FLAGS=-Wall -Wextra
DEBUG_FLAG=-g
INCLUDE_FLAG=-I $(INCLUDE_DIR)

CC=gcc
CFLAGS=$(WARNING_FLAGS) $(DEBUG_FLAG) $(INCLUDE_FLAG)
LDFLAGS=-L $(LIB_DIR)
LDLIBS=$(HEADER_FILE) -l hardware

BINARIES= ./mkhd/mkhd ./dmps/dmps ./frmt/frmt ./mbr/mbr ./part/part ./vm/vm

all : $(BINARIES)

clean:
	$(RM) *.o $(BINARIES)
realclean: clean 
	$(RM) vdiskA.bin vdiskB.bin
