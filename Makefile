
LIBRARIES=-lfftw3 -lsndfile -lstdc++fs

SOURCES=$(wildcard src/*.cpp)

OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES))

.PHONY = all clean

CX=g++
CXFLAGS=-g -Wall -std=c++17

all: bin/audio-filter

bin/audio-filter: $(OBJECTS)
	$(CX) $(CXFLAGS) -o bin/audio-filter $(OBJECTS) $(LIBRARIES)

obj/%.o: src/%.cpp
	$(CX) $(CXFLAGS) -c -o $@ $< $(LIBRARIES)

clean:
	rm -f obj/*.o
	rm -f bin/*