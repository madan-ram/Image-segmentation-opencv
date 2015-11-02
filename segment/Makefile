INCDIR = -I.
DBG    = -g
OPT    = -O3
CPP    = g++
CFLAGS = $(DBG) $(OPT) $(INCDIR)
LINK   = -lm -lboost_python -lboost_system

.cpp.o:
	$(CPP) $(CFLAGS) -c $< -o $@

all: segment

segment: segment.cpp segment-image.h segment-graph.h disjoint-set.h
	$(CPP) `pkg-config --cflags --libs opencv` `pkg-config --cflags --libs python2` \
-std=c++0x $(CFLAGS) \
-shared -o segment.so -fPIC segment.cpp $(LINK)
	
clean:
	/bin/rm -f segment *.o

clean-all: clean
	/bin/rm -f *~ 



