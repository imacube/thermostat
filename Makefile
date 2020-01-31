EXE := test

SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/utility/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS += -Iinclude -Wall
# CFLAGS += -Wall
# LDFLAGS += -Llib
# LDLIBS += -lm

ARDUINO_CONFIG := ./arduino/arduino-cli.yaml
# ARDUINO_CONFIG := ~/.arduino15/arduino-cli.yaml
ARDUINO_CORE := arduino:avr
ARDUINO_CLI := arduino-cli --config-file $(ARDUINO_CONFIG)

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifeq ($(uname_S),Darwin)
	CPPFLAGS += -Wno-c++11-extensions
endif

.PHONY: all clean

DEP = $(OBJ:.o=.d)

all: $(EXE)
	./test

-include $(DEP)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(OBJ_DIR)/utility
	$(CXX) $(CPPFLAGS) $< -MM -MT $(@:.d=.o) >$@

$(EXE): $(OBJ)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(OBJ_DIR)/utility
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(OBJ_DIR)/utility:
	mkdir -p $@

clean:
	$(RM) $(OBJ) $(DEP) $(EXE)

arduino-core-list:
	$(ARDUINO_CLI) core list

arduino-core-update:
	$(ARDUINO_CLI) core update-index

arduino-core-install: arduino-core-update
	$(ARDUINO_CLI) core list | \
	grep -q $(ARDUINO_CORE) || $(ARDUINO_CLI) core install $(ARDUINO_CORE)
