IDIR=include
CC=i686-w64-mingw32-gcc
CXX=i686-w64-mingw32-g++
CFLAGS=-I$(IDIR)

ODIR=src
LDIR=lib

LIBS=-lm -lpsapi

_DEPS=lowlvl.h losu.h summoner.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o low_functions.o search.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

floral_flyff_cheat.exe:  $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
