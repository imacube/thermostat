.PHONY: clean

# SOURCES := $(wildcard src/*.cpp)
SOURCES := src/test.cpp src/add.cpp
OBJECTS := $(subst .cpp,.o, $(SOURCES))

LIBRARY_SOURCES := $(wildcard mock/*.cpp) $(wildcard mock/utility/*.cpp)
LIBRARY_OBJECTS := $(subst .cpp,.o, $(LIBRARY_SOURCES))

test: $(LIBRARY_OBJECTS) $(OBJECTS)
	# $(CXX) -I mock -o $@ src/test.o
	$(CXX) -I $(LIBRARY_OBJECTS) -o $@ $(OBJECTS)

mock/%.o: mock/%.cpp
	$(COMPILE.cpp) -o $@ $<

src/%.o: src/%.cpp
	# $(COMPILE.cpp) -Wno-c++11-extensions -I mock -o $@ $<
	$(COMPILE.cpp) -fmax-errors=5 -I mock -o $@ $<

clean:
	-rm test $(LIBRARY_OBJECTS) $(OBJECTS)
