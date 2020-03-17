/*
 * esp01-at.cpp
 *
 *  Created on: Nov 2, 2019
 *      Author: Pim
 */

#include <Arduino.h>

#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#include "esp01-at.h"

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
  strcpy_P(_tx_buffer, (PGM_P)pgm_read_ptr(&(at_commands[command_id])));
  int length = strlen(_tx_buffer);

	if (format != NULL) {
		va_list args;
		va_start(args, format);
		vsprintf(_tx_buffer + length, format, args);
		length = strlen(_tx_buffer);
	}

  Serial.print(F("AT command: |"));
  Serial.write(_tx_buffer, length);
  Serial.println(F("|"));

	return (int)(_p_esp01Serial->write(_tx_buffer, length)) == length ? length : -1;
}

bool Esp01::isMessage(ESP01_MESSAGE_ID message, const char* data) {
	PGM_P message_text = (PGM_P)pgm_read_ptr(&(esp01_message[message]));
	return strncmp_P(data, message_text, strlen_P(message_text)) == 0 ? true : false;
}

/**
 * Receives the response on an AT command.
 * 
 * Returns the number of bytes received (0 in case no bytes available) 
 * or -1 if an error occured.
 */
int Esp01::atResponse(int (*response_handler)(char*, int), const bool wait) {
	do {
		while (_p_esp01Serial->available()) {
			*_rx++ = _p_esp01Serial->read();

			/*
			Serial.print(*rx, HEX);
			Serial.print(".");
			*/

			// The AT response followed by the sequence: "\r\n"
			if (*(_rx - 1) == '\n' && *(_rx - 2) == '\r' && (_rx - 2) - _rx_buffer > 0) {
				if (isMessage(OK, _rx - ESP01_MESSAGE_LENGTH(OK)) || 
						isMessage(ERROR, _rx - ESP01_MESSAGE_LENGTH(ERROR))) {
					int length = _rx - _rx_buffer;
					_rx = _rx_buffer;	// reset rx

					Serial.print(F("AT response: |"));
					Serial.write(_rx_buffer, length);
					Serial.println(F("|"));

					return response_handler == NULL 
						? atEmptyResponseHandler(_rx_buffer, length) 
						: response_handler(_rx_buffer, length);
				}	
				
				continue;
			}
		}

		if (wait) {
			delay(20);
		}
	} while (wait);

	return 0;
}

int Esp01::atEmptyResponseHandler(char* response, int length) {
	return isMessage(OK, response) ? length : -1;
}

int Esp01::dataReceive(int (*data_handler)(int connection, char*, int)) {
	while (_p_esp01Serial->available()) {
		*_rx = _p_esp01Serial->read();

		/*
				Serial.print(*_rx, HEX);
				Serial.print(".");
		*/

		if (*_rx == '\n' && *(_rx - 1) == '\r') {
			_rx++;

			int link;
			if (isMessage(NETWORK_CONNECT, _rx - ESP01_MESSAGE_LENGTH(NETWORK_CONNECT))) {
				_rx = _rx_buffer;

				link = atoi(strtok(_rx_buffer, ","));
				Serial.print("open connection: ");
				Serial.println(link);
			} 
			else if (isMessage(NETWORK_CLOSED, _rx - ESP01_MESSAGE_LENGTH(NETWORK_CLOSED))) {
				_rx = _rx_buffer;

				link = atoi(strtok(_rx_buffer, ","));
				Serial.print("close connection: ");
				Serial.println(link);
			} 
			else if (isMessage(NETWORK_IPD, _rx_buffer)) {
				Serial.print("data on connection: ");
				Serial.println(link);

				char* p = strtok(_rx_buffer, ",");
				p = strtok(NULL, ",");
				link = atoi(p);
				p = strtok(NULL, ",");
				int length = atoi(p);
				p = strtok(p, ":");
				char* data = strtok(NULL, ":");

				while(_rx - data < length) {
					if (_p_esp01Serial->available()) {
						*_rx = _p_esp01Serial->read();
						_rx++;
					}
				}

				_rx = _rx_buffer;

				return data_handler(link, data, length);
			} 
			else if (isMessage(SOFTAP_CONNECTED, _rx_buffer)) {
				_rx = _rx_buffer;

				Serial.print("ap connection");
			}	
			else if (isMessage(SOFTAP_DIST_IP, _rx_buffer)) {
				_rx = _rx_buffer;

				Serial.print("ap ip distribution");
			}
			else if (isMessage(SOFTAP_DISCONNECTED, _rx_buffer)) {
				_rx = _rx_buffer;

				Serial.print("ap disconnection");
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
	if (atCommand(IP_SEND, "=%d,%d\r\n", link, length) != -1) {
		delay(20);	// TODO wait for ">EOT" character sequence before sending the data.
		if (atResponse(NULL) != -1) {
			Serial.print("data: |");
			Serial.write(data, length);
			Serial.println("|");

			_p_esp01Serial->write(data, length);

			return length;
		};
	};

	return -1;
};

