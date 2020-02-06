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
ARDUINO_LIBRARIES := Arduino/libraries
ARDUINO_LIBRARIES_INSTALL = XBee-Arduino_library OneWire
ARDUINO_LIBRARIES_INSTALL := $(addprefix $(ARDUINO_LIBRARIES)/,$(ARDUINO_LIBRARIES_INSTALL))
ARDUINO_LIBRARIES_DOWNLOAD := XBeePayload Thermostat Adafruit_RGBLCDShield Adafruit_SleepyDog
ARDUINO_CORE_COUNT = $(shell $(ARDUINO_CLI) core list | grep -c $(ARDUINO_CORE) || true)

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

.PHONY: all clean arduino-core-install arduino-lib-install thermostat remote thermostat-upload remote-upload

DEP = $(OBJ:.o=.d)

all: $(EXE)
	./test
	$(MAKE) arduino-compile-thermostat

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
	$(RM) $(OBJ) $(DEP) $(EXE) $(addprefix Arduino/Thermostat/,*.elf *.hex)

arduino-core-install:
ifeq ($(ARDUINO_CORE_COUNT),0)
	$(ARDUINO_CLI) core update-index
	$(ARDUINO_CLI) core install $(ARDUINO_CORE)
endif

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
	mkdir -p $(ARDUINO_LIBRARIES)/Thermostat
	cp $(SRC_DIR)/Thermostat.cpp include/Thermostat.h $(ARDUINO_LIBRARIES)/Thermostat

thermostat: $(EXE) arduino-core-install arduino-lib-install
	./$(EXE)
	@$(ARDUINO_CLI) compile --fqbn $(ARDUINO_CORE):$(ARDUINO_MODEL) Arduino/Thermostat

remote: $(EXE) arduino-core-install arduino-lib-install
	./$(EXE)
	@$(ARDUINO_CLI) compile --fqbn $(ARDUINO_CORE):$(ARDUINO_MODEL) Arduino/Thermostat-Remote-Control

thermostat-upload: $(EXE) thermostat
	./$(EXE)
	$(ARDUINO_CLI) upload -p /dev/ttyACM0 --fqbn $(ARDUINO_CORE):$(ARDUINO_MODEL) Arduino/Thermostat

remote-upload: remote
	$(ARDUINO_CLI) upload -p /dev/ttyACM0 --fqbn $(ARDUINO_CORE):$(ARDUINO_MODEL) Arduino/Thermostat-Remote-Control
