EXE := test

SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/utility/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS += -Iinclude -Wall
# CFLAGS += -Wall
# LDFLAGS += -Llib
# LDLIBS += -lm

ARDUINO_CONFIG := ./.arduino/arduino-cli.yaml
ARDUINO_CORE := arduino:avr
ARDUINO_MODEL := uno
ARDUINO_CLI := $(HOME)/bin/arduino-cli --config-file $(ARDUINO_CONFIG)
ARDUINO_LIBRARIES:= Arduino/libraries
ARDUINO_LIBRARIES_INSTALL := XBee-Arduino_library OneWire
ARDUINO_LIBRARIES_INSTALL := $(addprefix $(ARDUINO_LIBRARIES)/,$(ARDUINO_LIBRARIES_INSTALL))
ARDUINO_LIBRARIES_DOWNLOAD := XBeePayload Thermostat Adafruit_RGBLCDShield Adafruit_SleepyDog

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifeq ($(uname_S),Darwin)
	CPPFLAGS += -Wno-c++11-extensions
endif

define download-lib
	echo $@
	-mkdir $(ARDUINO_LIBRARIES)
	curl -L -o $@.zip $(2)
	unzip -d $(ARDUINO_LIBRARIES) $@.zip
	mv $(ARDUINO_LIBRARIES)/$(1) $@
	touch $@
	rm $@.zip
endef

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

arduino-core-install:
	@$(MAKE) arduino-core-list | grep -q $(ARDUINO_CORE) || \
	($(MAKE) arduino-core-update && $(ARDUINO_CLI) core install $(ARDUINO_CORE))

arduino-lib-install: $(ARDUINO_LIBRARIES_INSTALL) $(addprefix $(ARDUINO_LIBRARIES)/,$(ARDUINO_LIBRARIES_DOWNLOAD))

$(ARDUINO_LIBRARIES_INSTALL):
	$(ARDUINO_CLI) lib install '$(subst _, ,$(notdir $@))'

$(ARDUINO_LIBRARIES)/XBeePayload:
	$(call download-lib,XBeePayload-master,https://github.com/imacube/XBeePayload/archive/master.zip)

$(ARDUINO_LIBRARIES)/Adafruit_RGBLCDShield:
	$(call download-lib,Adafruit-RGB-LCD-Shield-Library-1.0.3,https://github.com/imacube/Adafruit-RGB-LCD-Shield-Library/archive/1.0.3.zip)

$(ARDUINO_LIBRARIES)/Adafruit_SleepyDog:
	$(call download-lib,Adafruit_SleepyDog-1.2.0,https://github.com/imacube/Adafruit_SleepyDog/archive/1.2.0.zip)

$(ARDUINO_LIBRARIES)/Thermostat: $(SRC_DIR)/Thermostat.cpp include/Thermostat.h
	mkdir $(ARDUINO_LIBRARIES)/Thermostat
	cp $(SRC_DIR)/Thermostat.cpp include/Thermostat.h $(ARDUINO_LIBRARIES)/Thermostat

arduino-compile: arduino-core-install arduino-lib-install
	@$(ARDUINO_CLI) compile --fqbn $(ARDUINO_CORE):$(ARDUINO_MODEL) Arduino/Thermostat

arduino-clean:
	-rm $(addprefix Arduino/Thermostat/,*.elf *.hex)
