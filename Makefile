CC = g++
LIBS = -lSDL2
IDIR = include
BINC = $(IDIR)/includes.h
SDIR = src
BDIR = .
OBJDIR = .

DEPS = $(OBJDIR)/vector.o $(OBJDIR)/Mesh.o  $(OBJDIR)/display.o $(OBJDIR)/Camera.o $(OBJDIR)/utils.o $(OBJDIR)/Material.o $(OBJDIR)/Model.o

$(BDIR)/pillow: $(DEPS) $(OBJDIR)/main.o
	$(CC) -o $@ $^ $(LIBS)

$(OBJDIR)/Model.o: $(SDIR)/Model.cpp $(IDIR)/Model.h $(IDIR)/Resource.h $(BINC)
	$(CC) -c -o $@ $<

$(OBJDIR)/Material.o: $(SDIR)/Material.cpp $(IDIR)/Material.h $(IDIR)/Resource.h $(BINC)
	$(CC) -c -o $@ $<

$(OBJDIR)/utils.o:$(SDIR)/utils.cpp $(IDIR)/utils.h $(BINC)
	$(CC) -c -o $@ $<

$(OBJDIR)/vector.o: $(SDIR)/vector.cpp $(IDIR)/vector.h $(BINC)
	$(CC) -c -o $@ $<

$(OBJDIR)/Camera.o: $(SDIR)/Camera.cpp $(IDIR)/Camera.h $(BINC) $(OBJDIR)/vector.o $(BINC) 
	$(CC) -c -o $@ $<

$(OBJDIR)/display.o: $(SDIR)/display.cpp $(IDIR)/display.h $(BINC)
	$(CC) -c -o $@ $<

$(OBJDIR)/Mesh.o: $(SDIR)/Mesh.cpp $(IDIR)/Mesh.h $(IDIR)/Resource.h $(OBJDIR)/vector.o $(BINC) 
	$(CC) -c -o $@ $<

$(OBJDIR)/main.o: $(SDIR)/main.cpp $(DEPS) $(BINC)
	$(CC) -c -o $@ $<

clean:
	rm -f $(OBJDIR)/*.o core
