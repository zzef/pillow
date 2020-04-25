CC = g++
CFLAGS = -lSDL2
SDIR = src
BDIR = bin

$(BDIR)/pillow: 
	$(CC) -o $(BDIR)/pillow $(SDIR)/pillow.cpp $(CFLAGS)

