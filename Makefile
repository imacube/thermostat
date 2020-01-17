.PHONY: clean

SOURCES = $(wildcard src/*.cpp)
OBJECTS := $(subst .cpp,.o, $(SOURCES))

LIBRARY_SOURCES := $(wildcard mock/*.cpp) $(wildcard mock/utility/*.cpp)
LIBRARY_OBJECTS := $(subst .cpp,.o, $(LIBRARY_SOURCES))

test: $(LIBRARY_OBJECTS) $(OBJECTS)
	$(CXX) -I mock -o $@ src/test.o

mock/%.o: mock/%.cpp mock/%.h
	$(COMPILE.cpp) -o $@ $<

$(OBJECTS): $(SOURCES)
	$(COMPILE.cpp) -Wno-c++11-extensions -I mock -o $@ $<

clean:
	-rm test $(LIBRARY_OBJECTS) $(OBJECTS)
