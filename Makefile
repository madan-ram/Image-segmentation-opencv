INCDIR = -I.
DBG    = -g
OPT    = -O3
CPP    = g++
CFLAGS = $(DBG) $(OPT) $(INCDIR)
LINK   = -lm 

.cpp.o:
	$(CPP) $(CFLAGS) -c $< -o $@

all: segment

segment: segment.cpp segment-image.h segment-graph.h disjoint-set.h imagefile.h
	$(CPP) -std=c++0x $(CFLAGS) -o segment segment.cpp $(LINK) `pkg-config --cflags --libs opencv`

clean:
	/bin/rm -f segment *.o

clean-all: clean
	/bin/rm -f *~ 



