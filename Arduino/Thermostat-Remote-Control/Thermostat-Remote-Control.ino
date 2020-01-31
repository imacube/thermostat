/*
   Thermostat code, the control unit
*/
#include <XBee.h>
#include <SoftwareSerial.h>

#include "XBeePayload.h"

#include <Thermostat.h>

// #define DEBUG

#define SENDING_COUNT 2
#define GET_STATE_DELAY 17000
#define UPDATE_TEMP_SETTING_DELAY 2500
#define READ_PACKET_WAIT 2500
#define XBEE_DELAY 1000 // Add this to random number chosen by XBEE_DELAY_RANDOM_SIZE, all values in ms
#define XBEE_DELAY_RANDOM_SIZE 3000 // Random number window

#define MSG_TYPE_GET_STATE_RESPONSE 0x2 // Message type code for get_state response
#define MSG_TYPE_SEND_SETTINGS 0x3 // Message type code for sending settings

#define PACKET_NOTHING 0x0 // nothing set
#define PACKET_SUCCESS 0x1 // remote XBee received packet
#define PACKET_REMOTE_NOT_RECEIVED 0x2 // remote XBee did not receive packet
#define PACKET_ERROR 0x3 // Error reading packet received back from remote
#define PACKET_TX_STATUS_FAILURE 0x4 // Local XBee did not provided a timely TX status response
#define PACKET_NEITHER_RX_OR_ERROR 0x5 // Packet received is neither an error nor a received packet

Thermostat thermostat = Thermostat();

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40F51856); // Thermostat
// XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40F51858); // Thermostat
// create reusable response objects for responses we expect to handle
ZBExplicitRxResponse rx = ZBExplicitRxResponse();

// Define NewSoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
// Connect Arduino pin 9 to RX of usb-serial device
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial XBeeSerial(3, 2);

uint8_t i; // Used for for loops
unsigned long _get_state_last = millis() - GET_STATE_DELAY; // When state was last obtained from remote, using system millis()

struct thermostatStruct {
  uint8_t _temp; // Current temperature
  uint8_t _temp_setting; // Temperature setting
  uint8_t _heat; // On or Off?
  uint8_t _heat_relay; // On or Off?
  uint8_t _cool; // On or Off?
  uint8_t _cool_relay; // On or Off?
  uint8_t _fan_mode; // Auto or On?
  uint8_t _fan_relay; // On or Off?
  unsigned long _run_stop; // When the system turned off, in milliseconds
  unsigned long _run_start; // When the system turned on, in milliseconds
} current_settings;

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
  loading();
  thermostat.display_home();

  randomSeed(analogRead(0));
}

void loading() {
  /*
     Reload the data
  */
  uint8_t current_display = thermostat._current_display;
  uint8_t attempt = 0;
  thermostat.set_display(DISPLAY_OVERRIDE);
  thermostat._lcd.setCursor(0, 1);
  thermostat._lcd.print(F("Loading..."));
  thermostat._lcd.setCursor(0, 0);
  thermostat._lcd.print(F("Attempt: "));
  thermostat._lcd.print(attempt);
  while (!get_state()) {
    attempt++;
    thermostat._lcd.setCursor(0, 0);
    thermostat._lcd.print(F("Attempt: "));
    thermostat._lcd.print(attempt);
    Watchdog.reset();
    delay(XBEE_DELAY + random(XBEE_DELAY_RANDOM_SIZE));
    Watchdog.reset();
  }
  thermostat.set_display(current_display);
  thermostat.refresh();
}

void sending(int count) {
  /*
    Call sending() count times
  */
  for (uint8_t i = 0; i < count; i++) {
    sending();
  }
}

void sending() {
  /*
     Sending updated data to thermostat
  */
  uint8_t current_display = thermostat._current_display;
  uint8_t attempt = 0;
  thermostat.set_display(DISPLAY_OVERRIDE);
  thermostat._lcd.setCursor(0, 1);
  thermostat._lcd.print(F("Sending..."));
  thermostat._lcd.setCursor(0, 0);
  thermostat._lcd.print(F("Attempt: "));
  thermostat._lcd.print(attempt);
  while (!send_settings()) {
    attempt++;
    thermostat._lcd.setCursor(0, 0);
    thermostat._lcd.print(F("Attempt: "));
    thermostat._lcd.print(attempt);
    Watchdog.reset();
    delay(XBEE_DELAY + random(XBEE_DELAY_RANDOM_SIZE));
    Watchdog.reset();
  }
  thermostat.set_display(current_display);
  thermostat.refresh();
}

uint8_t send_packet(ZBTxRequest tx) {
  /*
    Send a packet
  */
  uint8_t return_status = PACKET_NOTHING;

  ZBTxStatusResponse txStatus = ZBTxStatusResponse();
  xbee.send(tx);

  for (i = 0; i < 5; i++) {
    #ifdef DEBUG
    Serial.print(F("Checking for RX packet: "));
    Serial.println(i);
    #endif
    Watchdog.reset();
    xbee.readPacket(READ_PACKET_WAIT);
    Watchdog.reset();

    if (xbee.getResponse().isAvailable()) {

      // should be a znet tx status
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);

        // get the delivery status, the fifth byte
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          #ifdef DEBUG
          Serial.println(F("The remote XBee received our packet"));
          #endif
          return_status = PACKET_SUCCESS;
          break;

        } else {
          #ifdef DEBUG
          Serial.println(F("The remote XBee did not receive our packet"));
          #endif
          return_status = PACKET_REMOTE_NOT_RECEIVED;
          break;
        }
      }
      else {
        #ifdef DEBUG
        Serial.print(F("Response unexpected, expected: "));
        Serial.println(ZB_TX_STATUS_RESPONSE, HEX);
        Serial.print(F("Got: "));
        Serial.println(xbee.getResponse().getApiId(), HEX);
        #endif
      }
    } else if (xbee.getResponse().isError()) {
      #ifdef DEBUG
      Serial.print(F("Error reading packet: "));
      Serial.println(xbee.getResponse().getErrorCode());
      #endif
      return_status = PACKET_ERROR;
      break;
    } else {
      #ifdef DEBUG
      Serial.println(F("Local XBee did not provide a timely TX Status Response -- should not happen"));
      #endif
      return_status = PACKET_TX_STATUS_FAILURE;
      break;
    }
  }

  Watchdog.reset();
  return return_status;
}

uint8_t get_requested_packet() {
  /*
    Capture a packet from a sent request
  */
  uint8_t return_status = PACKET_NOTHING;
  #ifdef DEBUG
  Serial.println(F("Check for the requested packet"));
  #endif

  Watchdog.reset();
  xbee.readPacket(READ_PACKET_WAIT);
  Watchdog.reset();

  if (xbee.getResponse().isAvailable()) {
    #ifdef DEBUG
    Serial.println(F("Got packet"));
    #endif

    if (xbee.getResponse().getApiId() == ZB_EXPLICIT_RX_RESPONSE) {
      // now fill our zb rx class
      xbee.getResponse().getZBExplicitRxResponse(rx);
      #ifdef DEBUG
      Serial.println(F("Got an rx packet!"));
      #endif

      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        // the sender got an ACK
        #ifdef DEBUG
        Serial.println(F("Packet acknowledged"));
        #endif
        return_status = PACKET_SUCCESS;
      } else {
        #ifdef DEBUG
        Serial.println(F("Packet not acknowledged"));
        #endif
        return_status = PACKET_REMOTE_NOT_RECEIVED;
      }

      #ifdef DEBUG
      Serial.print(F("Checksum is: "));
      Serial.println(rx.getChecksum(), HEX);
      #endif
    } else if (xbee.getResponse().isError()) {
      #ifdef DEBUG
      Serial.print(F("Error code:"));
      Serial.println(xbee.getResponse().getErrorCode());
      #endif
      return_status = PACKET_ERROR;
    }
    else {
      #ifdef DEBUG
      Serial.println(F("Neither expected packet or error packet"));
      #endif
      return_status = PACKET_NEITHER_RX_OR_ERROR;
    }
  }

  return return_status;
}

uint8_t get_state() {
  /*
    Query remote thermostat and use that data to set state
  */
  _get_state_last = millis();

  uint8_t remote_request[1];
  ZBTxRequest tx = ZBTxRequest(addr64, remote_request, sizeof(remote_request));
  remote_request[0] = 0x1;
  if (send_packet(tx) != PACKET_SUCCESS) {
    #ifdef DEBUG
    Serial.println(F("Send get_state packet failed"));
    #endif
    return false;
  }

  if (get_requested_packet() != PACKET_SUCCESS) {
    #ifdef DEBUG
    Serial.println(F("get_state response packet failed"));
    #endif
    return false;
  }

  // Verify proper message type
  #ifdef DEBUG
  Serial.print(F("Message type: "));
  Serial.println(rx.getData()[0], HEX);
  #endif
  if (rx.getData()[0] != MSG_TYPE_GET_STATE_RESPONSE) {
    #ifdef DEBUG
    Serial.print(F("Expected message type: "));
    Serial.println(MSG_TYPE_GET_STATE_RESPONSE);
    #endif
    return false;
  }

  // Get information from thermostat
  uint8_t crc_temp[sizeof(current_settings)];

  for (i = 2; i < rx.getDataLength(); i++) {
    crc_temp[i - 2] = rx.getData()[i];
  }

  // Calculate CRC
  if (rx.getData()[1] != crc_calc(crc_temp, sizeof(crc_temp))) {
    #ifdef DEBUG
    Serial.println(F("CRC missmatch"));
    #endif
    return false;
  }

  #ifdef DEBUG
  Serial.println(F("CRC Match"));
  #endif
  memcpy(&current_settings, &crc_temp, sizeof(current_settings));
  #ifdef DEBUG
  Serial.print(F("Temp: "));
  Serial.println(current_settings._temp);
  Serial.print(F("Temp Setting: "));
  Serial.println(current_settings._temp_setting);
  Serial.print(F("Heat Setting: "));
  Serial.println(current_settings._heat);
  Serial.print(F("Heat Relay: "));
  Serial.println(current_settings._heat_relay);
  Serial.print(F("Cool Setting: "));
  Serial.println(current_settings._cool);
  Serial.print(F("Cool Relay: "));
  Serial.println(current_settings._cool_relay);
  Serial.print(F("Fan Mode: "));
  Serial.println(current_settings._fan_mode);
  Serial.print(F("Fan Relay: "));
  Serial.println(current_settings._fan_relay);
  Serial.print(F("Run Stop: "));
  Serial.println(current_settings._run_stop);
  Serial.print(F("Run Start: "));
  Serial.println(current_settings._run_start);
  #endif

  thermostat.set_temp(current_settings._temp);
  thermostat._temp_setting = current_settings._temp_setting;
  thermostat._heat = current_settings._heat;
  thermostat._heat_relay = current_settings._heat_relay;
  thermostat._cool = current_settings._cool;
  thermostat._cool_relay = current_settings._cool_relay;
  thermostat._fan_mode = current_settings._fan_mode;
  thermostat._fan_relay = current_settings._fan_relay;
  thermostat._run_stop = current_settings._run_stop;
  thermostat._run_start = current_settings._run_start;

  return true;
}

uint8_t send_settings() {
  /*
     Send updated settings to remote
  */
  #ifdef DEBUG
  Serial.println(F("Send updated settings to remote"));
  #endif

  struct settingsStruct {
    uint8_t _temp_setting; // Temperature setting
    uint8_t _heat; // On or Off?
    uint8_t _cool; // On or Off?
    uint8_t _fan_mode; // Auto or On?
  } settings_struct;

  settings_struct._temp_setting = thermostat._temp_setting;
  settings_struct._heat = thermostat._heat;
  settings_struct._cool = thermostat._cool;
  settings_struct._fan_mode = thermostat._fan_mode;

  uint8_t settings_data[sizeof(settings_struct)];
  memcpy(&settings_data, &settings_struct, sizeof(settings_struct));

  uint8_t payload[sizeof(settings_struct) + 2];
  gen_payload(payload, sizeof(payload), MSG_TYPE_SEND_SETTINGS, settings_data, sizeof(settings_data));

  ZBTxRequest tx = ZBTxRequest(addr64, payload, sizeof(payload));
  if (send_packet(tx) != PACKET_SUCCESS) {
    #ifdef DEBUG
    Serial.println(F("Send send_settings packet failed"));
    #endif
    return false;
  }

  return true;
}

uint8_t check_temp() {
  /*
     Compare previous temperature setting to current setting.
     If our local copy has changed send that to remote thermostat.
  */
  if (thermostat._temp_setting != current_settings._temp_setting) {
    return true;
  }
  return false;
}

uint8_t check_settings() {
  /*
     Compare previous settings to current settings.
     If our local copy has changed send that to remote thermostat.
  */
  if (thermostat._heat != current_settings._heat ||
      thermostat._cool != current_settings._cool ||
      thermostat._fan_mode != current_settings._fan_mode) {
    return true;
  }
  return false;
}

void loop() {
  // Update thermostat setting if updated, don't delay
  if (check_settings()) {
    sending(SENDING_COUNT);
    loading();
  }
  // Update temp setting if idle for UPDATE_TEMP_SETTING_DELAY
  if (check_temp() && millis() - thermostat._idle > UPDATE_TEMP_SETTING_DELAY) {
    sending(SENDING_COUNT);
    loading();
    thermostat._idle = millis();
  }
  if (millis() - _get_state_last > GET_STATE_DELAY) {
    if (check_temp() || check_settings()) {
      sending(SENDING_COUNT);
    }
    loading(); // Get the current state from the remote thermostat
  }
  thermostat.yield();
}
