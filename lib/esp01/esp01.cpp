/*
 * at-command.cpp
 *
 *  Created on: Nov 2, 2019
 *      Author: Pim
 */

#include <Arduino.h>

#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#include "esp01.h"

Esp01::Esp01() {
	_p_esp01Serial = new SoftwareSerial(5, 6);
	_p_esp01Serial->begin(9600);
}

Esp01::~Esp01() {
	delete(_p_esp01Serial);
}

/**
 * Send AT-command to ESP-01 module.
 * 
 * Returns number of bytes send, or -1 if an error occured
 */
int Esp01::atCommand(AT_COMMAND_ID command_id) {
	return atCommand(command_id, NULL);
}

int Esp01::atCommand(AT_COMMAND_ID command_id, const char* format, ...) {
  memset(_tx_buffer, '\0', TX_BUFFER_SIZE);
  strcpy_P(_tx_buffer, (PGM_P)pgm_read_word(&(at_commands[command_id])));
  _tx_length = at_command_lengths[command_id];

	if (format != NULL) {
		va_list args;
		va_start(args, format);
		vsprintf(_tx_buffer + _tx_length, format, args);
		_tx_length = strlen(_tx_buffer);
	}

  Serial.print("tx_buffer: |");
  Serial.write(_tx_buffer, _tx_length);
  Serial.println("|");

	return (int)(_p_esp01Serial->write(_tx_buffer, _tx_length)) == _tx_length ? _tx_length : -1;
}

bool Esp01::isMessage(ESP01_MESSAGE message, const char* data) {
	return strncmp_P(data, (PGM_P)pgm_read_word(&(esp01_message[message])), esp01_message_lengths[message] ) == 0 ? true : false;
}


/**
 * Receives the response on an AT command.
 * 
 * Returns the number of bytes received (0 in case no bytes available) 
 * or -1 if an error occured.
 */
int Esp01::atResponse(int (*response_handler)(const char*, const int), const bool wait) {
	do {
		while (_p_esp01Serial->available()) {
			*_rx = _p_esp01Serial->read();

			/*
			Serial.print(*rx, HEX);
			Serial.print(".");
			*/

			// Echo of the AT command followed by the sequence: "\r\r\n"
			if (*_rx == '\n' && *(_rx - 1) == '\r' && *(_rx -2) == '\r') {
				memset(_rx_buffer, '\0', RX_BUFFER_SIZE);
				_rx = _rx_buffer;

				continue;
			}

			// The AT response followed by the sequence: "\r\n"
			if (*_rx == '\n' && *(_rx - 1) == '\r' && (_rx - 1) - _rx_buffer > 0) {
				_rx++;

				if (isMessage(OK, _rx - esp01_message_lengths[OK]) || 
						isMessage(ERROR, _rx - esp01_message_lengths[ERROR])) {
					_rx_length = _rx - _rx_buffer;
					_rx = _rx_buffer;	// reset rx

					Serial.print("rx_buffer: |");
					Serial.write(_rx_buffer, _rx_length);
					Serial.println("|");

					return response_handler == NULL 
						? atEmptyResponseHandler(_rx_buffer, _rx_length) 
						: response_handler(_rx_buffer, _rx_length);
				}	
				
				continue;
			}

			// ... store the character
			_rx++;
		}

		if (wait) {
			delay(20);
		}

	} while (wait);

	return 0;
}

int Esp01::atEmptyResponseHandler(const char* response, const int length) {
	return isMessage(ERROR, response) ? -1 : length;
}

int Esp01::dataReceive(int (*data_handler)(int connection, const char*, const int)) {
	while (_p_esp01Serial->available()) {
		*_rx = _p_esp01Serial->read();

		/*
				Serial.print(*rx, HEX);
				Serial.print(".");
		*/

		if (*_rx == '\n' && *(_rx - 1) == '\r') {
			_rx++;

			_rx_length = _rx - _rx_buffer;
			Serial.print("rx_buffer: |");
			Serial.write(_rx_buffer, _rx_length);
			Serial.println("|");

			int link;
			if (isMessage(NETWORK_CONNECT, _rx - esp01_message_lengths[NETWORK_CONNECT])) {
				_rx = _rx_buffer;

				link = atoi(strtok(_rx_buffer, ","));
				Serial.print("open connection: ");
				Serial.println(link);
			} else if (isMessage(NETWORK_CLOSED, _rx - esp01_message_lengths[NETWORK_CLOSED])) {
				_rx = _rx_buffer;

				link = atoi(strtok(_rx_buffer, ","));
				Serial.print("close connection: ");
				Serial.println(link);
			} else if (isMessage(NETWORK_IPD, _rx_buffer)) {
				_rx = strtok(_rx_buffer, ","); // rx -> "+IPD"
				_rx = strtok(NULL, ","); // rx -> connection
				link = atoi(_rx);
				_rx = strtok(NULL, ","); // rx -> length
				int length = atoi(_rx);
				_rx = strtok(_rx, ":");	// rx -> length
				char* data = strtok(NULL, ":"); // rx -> data

				Serial.print("data on connection: ");
				Serial.println(link);

				_rx = _rx_buffer;

				return data_handler(link, data, length - 2);
			}
			else {
				_rx = _rx_buffer;			
			}

			continue;
		}

		_rx++;
	}

	return 0;
};

int Esp01::dataSend(const int link, const char* data, const int length) {
	if (atCommand(IP_SEND, "=%d,%d\r\n", link, length + 2) != -1) {
		delay(20);	// TODO wait for ">EOT" character sequence before sending the data.
		if (atResponse(NULL) != -1) {
			Serial.print("data: |");
			Serial.write(data, length);
			Serial.write("\r\n");
			Serial.println("|");

			_p_esp01Serial->write(data, length);
			_p_esp01Serial->write("\r\n");

			return length + 2;
		};
	};

	return -1;
};

