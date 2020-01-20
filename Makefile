EXE = test

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/utility/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS += -Iinclude -Wall
# CFLAGS += -Wall
# LDFLAGS += -Llib
# LDLIBS += -lm

.PHONY: all clean

DEP = $(OBJ:.o=.d)

all: $(EXE)
	./test

-include $(DEP)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $< -MM -MT $(@:.d=.o) >$@

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(DEP) $(EXE)
