CC = g++
FLAGS = -O3
LIBS = -lSDL2
IDIR = include
BINC = $(IDIR)/includes.h
SDIR = src
BDIR = .
OBJDIR = .

DEPS = $(OBJDIR)/vector.o $(OBJDIR)/Mesh.o $(OBJDIR)/Mesh_loader.o  $(OBJDIR)/display.o 

$(BDIR)/pillow: $(DEPS) $(OBJDIR)/main.o
	$(CC) -o $@ $^ $(LIBS) $(FLAGS)

$(OBJDIR)/vector.o: $(SDIR)/vector.cpp $(IDIR)/vector.h $(BINC)
	$(CC) -c -o $@ $<

$(OBJDIR)/display.o: $(SDIR)/display.cpp $(IDIR)/display.h $(BINC)
	$(CC) -c -o $@ $<

$(OBJDIR)/Mesh.o: $(SDIR)/Mesh.cpp $(IDIR)/Mesh.h $(OBJDIR)/vector.o $(BINC) 
	$(CC) -c -o $@ $<

$(OBJDIR)/Mesh_loader.o: $(SDIR)/Mesh_load.cpp $(IDIR)/Mesh_load.h $(OBJDIR)/Mesh.o $(BINC) 
	$(CC) -c -o $@ $<

$(OBJDIR)/main.o: $(SDIR)/main.cpp $(DEPS) $(BINC)
	$(CC) -c -o $@ $<

clean:
	rm -f $(OBJDIR)/*.o core
