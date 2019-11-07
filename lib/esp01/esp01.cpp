/*
 * at-command.cpp
 *
 *  Created on: Nov 2, 2019
 *      Author: Pim
 */

#include <Arduino.h>

#include <HardwareSerial.h>

#include <SoftwareSerial.h>
static SoftwareSerial esp01Serial = SoftwareSerial(5, 6);

#include "esp01.h"

#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

static char tx_buffer[TX_BUFFER_SIZE];
static size_t tx_length;
static char rx_buffer[RX_BUFFER_SIZE];
static size_t rx_length;
static char* rx = rx_buffer;

void esp01Init() {
	esp01Serial.begin(9600);
}

size_t esp01Command(AT_COMMAND_ID command_id) {
  memset(tx_buffer, '\0', TX_BUFFER_SIZE);
  strcpy_P(tx_buffer, (PGM_P)pgm_read_word(&(at_commands[command_id])));
  tx_length = at_command_lengths[command_id];

  Serial.print("tx_buffer: |");
  Serial.write(tx_buffer, tx_length);
  Serial.println("|");

  return esp01Serial.write(tx_buffer, tx_length);
}

ESP01_STATE esp01Response(ESP01_STATE current_state, ESP01_STATE (*response_handler)(const char*, const size_t)) {
	while (esp01Serial.available()) {
		*rx = esp01Serial.read();

		/*
		Serial.print(*rx, HEX);
		Serial.print(".");
		*/

		// Echo of the AT command followed by the sequence: "\r\r\n"
		if (*rx == '\n' && *(rx - 1) == '\r' && *(rx -2) == '\r') {
			memset(rx_buffer, '\0', RX_BUFFER_SIZE);
			rx = rx_buffer;

			continue;
		}

		// The AT response followed by the sequence: "\r\n"
		if (*rx == '\n' && *(rx - 1) == '\r' && (rx - 1) - rx_buffer > 0) {
			rx++;

			if (isResponseStatus(OK, rx - response_status_lengths[OK])) {
				rx_length = rx - rx_buffer;
				Serial.print("rx_buffer: |");
				Serial.write(rx_buffer, rx_length);
				Serial.println("|");

				rx = rx_buffer;
				return response_handler(rx_buffer, rx_length);
			}

			continue;
		}

		// ... store the character
		rx++;
	}

	return current_state;
}

ESP01_STATE esp01Listen(ESP01_STATE (*data_handler)(const char*, const size_t)) {
	while (esp01Serial.available()) {
		*rx = esp01Serial.read();

		Serial.print(*rx, HEX);
		Serial.print(".");

		if (*rx == '\n' && *(rx - 1) == '\r') {
			rx++;

			rx_length = rx - rx_buffer;
			Serial.print("rx_buffer: |");
			Serial.write(rx_buffer, rx_length);
			Serial.println("|");

			rx = rx_buffer;
			return data_handler(rx_buffer, rx_length);
		}

		rx++;
	}

	return LISTEN;
};
