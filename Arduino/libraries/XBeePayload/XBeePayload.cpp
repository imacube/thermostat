/*
  XBeePayload.cpp - A helper method to prepare a byte array for transmission via XBee radios.
*/

#include "XBeePayload.h"

uint8_t crc_calc(uint8_t *data, uint16_t data_length) {
  /*
     Calculate a CRC-8 of the object passed

     Source: https://www.maximintegrated.com/en/app-notes/index.mvp/id/27
  */

  uint8_t crc = 0x0;

  for (uint16_t i = 0; i < data_length; i++) {
    crc = crc_table[crc ^ data[i]];
  }

  return crc;
}

void *gen_payload(uint8_t *payload, uint8_t payload_size, uint8_t msg_type, uint8_t *data, uint16_t data_size) {
  /*
  Generate the payload array, including the CRC calculation
  */

  for (int i = 0; i < data_size; i++) {
    payload[i + 2] = data[i];
  }

  payload[0] = msg_type;
  payload[1] = crc_calc(data, data_size);
}
