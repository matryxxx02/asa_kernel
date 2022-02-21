###------------------------------
### Configuration 
###------------------------------------------------------------

CC	= gcc
CFLAGS	= -m32 -fno-pic
CFLAGS  += -Wall -pedantic 
# CFLAGS  += -ansi
CFLAGS  += -g

###------------------------------
### Dump0 rules 
###------------------------------------------------------------
dump0 :dump0.o
	$(CC) $(CFLAGS) -o dump0 dump0.o
dump0.o :dump0.c 
	$(CC) $(CFLAGS) -c dump0.c 

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o 
realclean: clean 
	$(RM) dump0
