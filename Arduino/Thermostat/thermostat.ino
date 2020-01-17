/*
   Thermostat code, the control unit
*/
#include <XBee.h>
#include <SoftwareSerial.h>

#include "XBeePayload.h"

#include <Thermostat.h>

#define DEBUG

#define TEMP_MILLIS_INTERVAL 120000 // How long since an altnerate temperature was set

Thermostat thermostat = Thermostat();

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBExplicitRxResponse rx = ZBExplicitRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

// Define NewSoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
// Connect Arduino pin 9 to RX of usb-serial device
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial XBeeSerial(3, 2);

uint8_t i; // Used for for loops

unsigned long temp_millis; // Alternate temp source

void setup() {
  // start serial
  Serial.begin(9600);
  XBeeSerial.begin(9600);
  xbee.setSerial(XBeeSerial);
  Serial.begin(9600);

#ifdef DEBUG
  Serial.println(F("Starting up!"));
#endif

  thermostat.init((uint8_t) 16, (uint8_t) 2);

  thermostat.set_temp((uint8_t) 66);
  thermostat.display_home();

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_COOL_1, OUTPUT);
  pinMode(RELAY_COOL_2, OUTPUT);
  pinMode(RELAY_HEAT, OUTPUT);

  thermostat.set_temp_setting((uint8_t) 75);
//  thermostat.on_cool();

  temp_millis = millis() - TEMP_MILLIS_INTERVAL;
}

void receive_state() {
  /*
     Receive state and change the settings
  */

  uint8_t data_crc = 0x0;

  struct settingsStruct {
    uint8_t temp_setting; // Temperature setting
    uint8_t heat; // On or Off?
    uint8_t cool; // On or Off?
    uint8_t fan_mode; // Auto or On?
  } settings_struct;

  // Convert data payload back to struct above
  uint8_t settings_temp[sizeof(settings_struct)];

  for (i = 2; i < rx.getDataLength(); i++) {
    settings_temp[i - 2] = rx.getData()[i];
  }

  data_crc = rx.getData()[1];
  memcpy(&settings_struct, &settings_temp, sizeof(settings_struct));

  // Verify checksum
  if (data_crc != crc_calc(settings_temp, sizeof(settings_temp))) {
    return;
  }

#ifdef DEBUG
  Serial.print(F("Temp Setting: "));
  Serial.println(settings_struct.temp_setting);
  Serial.print(F("Heat Setting: "));
  Serial.println(settings_struct.heat);
  Serial.print(F("Cool Setting: "));
  Serial.println(settings_struct.cool);
  Serial.print(F("Fan Mode: "));
  Serial.println(settings_struct.fan_mode);
#endif

  // Both Hot and Cool turned on remotly, abort!
  if (settings_struct.heat == true && settings_struct.cool == true) {
    return;
  }

  // Change settings
  thermostat.set_temp_setting(settings_struct.temp_setting);

  if (thermostat._cool != settings_struct.cool) {
    if (settings_struct.cool) {
      thermostat.on_cool();
    }
    else {
      thermostat.off_cool();
    }
  }
  if (thermostat._heat != settings_struct.heat) {
    if (settings_struct.heat) {
      thermostat.on_heat();
    }
    else {
      thermostat.off_heat();
    }
  }

  if (thermostat._fan_mode != settings_struct.fan_mode) {
    if (settings_struct.fan_mode) {
      thermostat.on_fan();
    }
    else {
      thermostat.auto_fan();
    }
  }

  thermostat.refresh();
}

void send_state() {
  /*
     Send back the current Thermostat State
  */

  struct thermostatStruct {
    uint8_t _temp; // Current temperature
    uint8_t _temp_setting; // Temperature setting
    uint8_t _heat; // On or Off?
    uint8_t _heat_relay; // On or Off?
    uint8_t _cool; // On or Off?
    uint8_t _cool_relay; // On or Off?
    uint8_t _fan_mode; // Auto or On?
    uint8_t _fan_relay; // On or Off?
    unsigned long _run_stop; // When the system turned off, using system millis()
    unsigned long _run_start; // When the system turned on, using system millis()
  } thermostat_struct;

  uint8_t payload[sizeof(thermostat_struct) + 2];

  XBeeAddress64 remoteAddress;

  remoteAddress = rx.getRemoteAddress64();

  XBeeAddress64 addr64 = XBeeAddress64(remoteAddress.getMsb(), remoteAddress.getLsb()); // Sender to reply to
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();

  thermostat_struct._temp = thermostat._temp;
  thermostat_struct._temp_setting = thermostat._temp_setting;
  thermostat_struct._heat = thermostat._heat;
  thermostat_struct._heat_relay = thermostat._heat_relay;
  thermostat_struct._cool = thermostat._cool;
  thermostat_struct._cool_relay = thermostat._cool_relay;
  thermostat_struct._fan_mode = thermostat._fan_mode;
  thermostat_struct._fan_relay = thermostat._fan_relay;
  thermostat_struct._run_stop = millis() - thermostat._run_stop;
  thermostat_struct._run_start = millis() - thermostat._run_start;

  uint8_t thermostat_data[sizeof(thermostat_struct)];
  memcpy(&thermostat_data, &thermostat_struct, sizeof(thermostat_struct));
  gen_payload(payload, sizeof(payload), 0x02, thermostat_data, sizeof(thermostat_data));

  xbee.send(zbTx);

  if (xbee.readPacket(500)) {
    // should be a znet tx status
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
    }
  } else if (xbee.getResponse().isError()) {
  } else {
    Serial.println(F("local XBee did not provide a timely TX Status Response -- should not happen"));
  }

}

void receive_temp() {
  /*
     Receive state and change the settings
  */

  uint8_t data_crc = 0x0;

  struct tempStruct {
    uint8_t temp; // Temperature
    uint8_t source; // Source of temperature
  } temp_struct;

  // Convert data payload back to struct above
  uint8_t temp_data[sizeof(temp_struct)];

  for (i = 2; i < rx.getDataLength(); i++) {
    temp_data[i - 2] = rx.getData()[i];
  }

  data_crc = rx.getData()[1];
  memcpy(&temp_struct, &temp_data, sizeof(temp_struct));

  // Verify checksum
  if (data_crc != crc_calc(temp_data, sizeof(temp_data))) {
    return;
  }

  Serial.print(F("Temp: "));
  Serial.println(temp_struct.temp);
  Serial.print(F("Source: "));
  Serial.println(temp_struct.source);

  // Process the temperature sent
  uint8_t temp; // Stores temperature data
  uint8_t sender; // What sent the temperature

  thermostat.set_temp(temp_struct.temp);
  temp_millis = millis();
}

void process_temp() {
  // Process the temperature sent
  byte data[12]; // Stores temperature data
  float fahrenheit; // Converted temperature

  fahrenheit = -999; // Default value, checked for when choosing to set temperature or not

  // Get the temperature data
  for (i = 0; i < sizeof(data); i++) {
    data[i] = rx.getData()[i + 1];
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];

  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit rxes, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  // default is 12 bit resolution, 750 ms conversion time

  fahrenheit = ((float)raw / 16.0) * 1.8 + 32.0;
#ifdef DEBUG
  Serial.print(F("  Temperature = "));
  Serial.print(fahrenheit);
  Serial.println(F(" Fahrenheit"));

  Serial.print(F("float: "));
  Serial.println(fahrenheit);
  Serial.print(F("uint8_t: "));
  Serial.println((uint8_t) fahrenheit);
#endif
  if (fahrenheit != -999) {
    if (fahrenheit > 65 && fahrenheit < 85) {
      thermostat.set_temp((uint8_t) fahrenheit);
    }
  }
}

void loop() {
  byte sender_id;

  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something
    if (xbee.getResponse().getApiId() == ZB_EXPLICIT_RX_RESPONSE) {
      xbee.getResponse().getZBExplicitRxResponse(rx);
    }

    // Get room identifier
    sender_id = rx.getData()[0];
    #ifdef DEBUG
    Serial.print(F("Room ID: "));
    Serial.print(sender_id, HEX);
    #endif
    if (sender_id == 0x2B) {
      #ifdef DEBUG
      Serial.println(F(" for process_temp()"));
      #endif

      if (millis() - temp_millis >= TEMP_MILLIS_INTERVAL) {
        process_temp();
      }
    }
    else if (sender_id == 0x01) {
      #ifdef DEBUG
      Serial.println(F(" for send_state()"));
      #endif
      send_state();
    }
    else if (sender_id == 0x03) {
      #ifdef DEBUG
      Serial.println(F(" for receive_state()"));
      #endif
      receive_state();
    }
    else if (sender_id >= 0x10 and sender_id <= 0x1F) {
      // Set the temperature from an alternate source
      receive_temp();
    }

  } else if (xbee.getResponse().isError()) {
#ifdef DEBUG
    Serial.print(F("error code:"));
    Serial.println(xbee.getResponse().getErrorCode());
#endif
  }

  // Verify thermostat temperature is changing, i.e. the control signal is going out
  if (thermostat._cool_relay == ON && millis() - thermostat._run_start >= 300000 && thermostat._temp > thermostat._temp_start) {
    Watchdog.reset();
    // Cycle relay off for 10 seconds
    thermostat.cool_relay(OFF);

    // Watchdog reset and loop for 10000 ms
    for (i = 0; i < 10; i++) {
      delay(1000);
      Watchdog.reset();
    }
    // Cycle relay on
    thermostat.cool_relay(ON);
    Watchdog.reset();
  }

  thermostat.yield();
}
