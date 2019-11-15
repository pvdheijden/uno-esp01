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
static int tx_length;
static char rx_buffer[RX_BUFFER_SIZE];
static int rx_length;
static char* rx = rx_buffer;

void esp01Init() {
	esp01Serial.begin(9600);
}

int esp01Command(AT_COMMAND_ID command_id, const char* format, ...) {
  memset(tx_buffer, '\0', TX_BUFFER_SIZE);
  strcpy_P(tx_buffer, (PGM_P)pgm_read_word(&(at_commands[command_id])));
  tx_length = at_command_lengths[command_id];

	if (format != NULL) {
		va_list args;
		va_start(args, format);
		vsprintf(tx_buffer + tx_length, format, args);
		tx_length = strlen(tx_buffer);
	}

  Serial.print("tx_buffer: |");
  Serial.write(tx_buffer, tx_length);
  Serial.println("|");

	return esp01Serial.write(tx_buffer, tx_length) == tx_length ? tx_length : -1;
}

ESP01_STATE esp01Response(ESP01_STATE current_state, ESP01_STATE next_state) {
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

			if (isEsp01Message(OK, rx - esp01_message_lengths[OK])) {
				rx_length = rx - rx_buffer;
				rx = rx_buffer;	// reset rx

				Serial.print("rx_buffer: |");
				Serial.write(rx_buffer, rx_length);
				Serial.println("|");

				return next_state;
			}

			continue;
		}

		// ... store the character
		rx++;
	}

	return current_state;
}

ESP01_STATE esp01Receive(ESP01_STATE (*data_handler)(int connection, const char*, const int)) {
	while (esp01Serial.available()) {
		*rx = esp01Serial.read();

/*
		Serial.print(*rx, HEX);
		Serial.print(".");
*/

		if (*rx == '\n' && *(rx - 1) == '\r') {
			rx++;

			rx_length = rx - rx_buffer;
			Serial.print("rx_buffer: |");
			Serial.write(rx_buffer, rx_length);
			Serial.println("|");

			int link;
			if (isEsp01Message(NETWORK_CONNECT, rx - esp01_message_lengths[NETWORK_CONNECT])) {
				rx = rx_buffer;

				link = atoi(strtok(rx_buffer, ","));
				Serial.print("open connection: ");
				Serial.println(link);
			} else if (isEsp01Message(NETWORK_CLOSED, rx - esp01_message_lengths[NETWORK_CLOSED])) {
				rx = rx_buffer;

				link = atoi(strtok(rx_buffer, ","));
				Serial.print("close connection: ");
				Serial.println(link);
			} else if (isEsp01Message(NETWORK_IPD, rx_buffer)) {
				rx = strtok(rx_buffer, ","); // rx -> "+IPD"
				rx = strtok(NULL, ","); // rx -> connection
				link = atoi(rx);
				rx = strtok(NULL, ","); // rx -> length
				int length = atoi(rx);
				rx = strtok(rx, ":");	// rx -> length
				char* data = strtok(NULL, ":"); // rx -> data

				Serial.print("data on connection: ");
				Serial.println(link);

				rx = rx_buffer;

				return data_handler(link, data, length - 2);
			}
			else {
				rx = rx_buffer;			
			}

			continue;
		}

		rx++;
	}

	return RECEIVE;
};

ESP01_STATE esp01Send(const int link, const char* data, const int length) {
	if (esp01Command(IP_SEND, "=%d,%d\r\n", link, length + 2) != -1) {
		delay(20);	// TODO wait for ">EOT" character sequence before sending the data.
		esp01Response(RECEIVE, RECEIVE);

		Serial.print("data: |");
	  Serial.write(data, length);
		Serial.write("\r\n");
  	Serial.println("|");

		esp01Serial.write(data, length);
		esp01Serial.write("\r\n");
	};

	return RECEIVE;
};

