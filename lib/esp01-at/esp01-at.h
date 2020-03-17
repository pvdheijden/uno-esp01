/*
 * esp01-at.h
 *
 *  Created on: Nov 2, 2019
 *      Author: Pim
 */

#ifndef ESP01_AT_H_
#define ESP01_AT_H_

#include <Arduino.h>
#include <string.h>
/**
 * AT command definitions
 */
typedef enum _AT_COMMAND_ID {
	AT = 0,
	RESTART = 1,
	VERSION = 2,
	STA_MODE = 3,
	SOFTAP_MODE = 4,
	WIFI_CHECK = 5,
	WIFI_CONNECT = 6,
	IP_MUX_ON = 7,
	IP_SERVER_ON = 8,
	IP_SEND = 9
} AT_COMMAND_ID;

static const char AT_command[] PROGMEM = "AT\r\n";
static const char RESTART_command[] PROGMEM = "AT+RST\r\n";
static const char VERSION_command[] PROGMEM = "AT+GMR\r\n";
static const char STA_MODE_command[] PROGMEM = "AT+CWMODE=1\r\n";
static const char SOFTAP_MODE_command[] PROGMEM = "AT+CWMODE=3\r\n";
static const char WIFI_CHECK_command[] PROGMEM = "AT+CIFSR\r\n";
static const char WIFI_CONNECT_command[] PROGMEM = "AT+CWJAP=\"NETGEAR34\",\"smilingowl440\"\r\n";
static const char IP_MUX_ON_command[] PROGMEM = "AT+CIPMUX=1\r\n";
static const char IP_SERVER_ON_command[] PROGMEM = "AT+CIPSERVER=1,3333\r\n";
static const char IP_SEND_command[] PROGMEM = "AT+CIPSENDEX";

PGM_P const at_commands[] PROGMEM = {
	AT_command,
	RESTART_command,
	VERSION_command,
	STA_MODE_command,
	SOFTAP_MODE_command,
	WIFI_CHECK_command,
	WIFI_CONNECT_command,
	IP_MUX_ON_command,
	IP_SERVER_ON_command,
	IP_SEND_command
};

#define AT_COMMAND_LENGTH(ID) (strlen(at_commands[ID]))

typedef enum _ESP01_MESSAGE_ID {
	OK = 0,
	READY = 1,
	ERROR = 2,
	WIFI_CONNECTED = 3,
	WIFI_GOT_IP = 4,
	WIFI_DISCONNECT = 5,
	BUSY_S = 6,
	BUSY_P = 7,
	NETWORK_CONNECT = 8,
	NETWORK_CLOSED = 9,
	NETWORK_IPD = 10,
	SOFTAP_CONNECTED = 11,
	SOFTAP_DIST_IP = 12,
	SOFTAP_DISCONNECTED = 13
} ESP01_MESSAGE_ID;

static const char OK_message[] PROGMEM = "OK\r\n";
static const char READY_message[] PROGMEM = "ready\r\n";
static const char ERROR_message[] PROGMEM = "ERROR\r\n";
static const char WIFI_CONNECTED_message[] PROGMEM = "WIFI CONNECTED\r\n";
static const char WIFI_GOT_IP_message[] PROGMEM = "WIFI GOT IP\r\n";
static const char WIFI_DISCONNECT_message[] PROGMEM = "WIFI DISCONNECT\r\n";
static const char BUSY_S_message[] PROGMEM = "busy s...\r\n";
static const char BUSY_P_message[] PROGMEM = "busy p...\r\n";
static const char NETWORK_CONNECT_message[] PROGMEM = ",CONNECT\r\n";
static const char NETWORK_CLOSED_message[] PROGMEM = ",CLOSED\r\n";
static const char NETWORK_IPD_message[] PROGMEM = "+IPD";
static const char SOFTAP_CONNECTED_message[] PROGMEM = "+STA_CONNECTED:";
static const char SOFTAP_DIST_IP_message[] PROGMEM = "+DIST_STA_IP:";
static const char SOFTAP_DISCONNECTED_message[] PROGMEM = "+STA_DISCONNECTED:";

PGM_P const esp01_message[] PROGMEM = {
	OK_message,
	READY_message,
	ERROR_message,
	WIFI_CONNECTED_message,
	WIFI_GOT_IP_message,
	WIFI_DISCONNECT_message,
	BUSY_S_message,
	BUSY_P_message,
	NETWORK_CONNECT_message,
	NETWORK_CLOSED_message,
	NETWORK_IPD_message,
	SOFTAP_CONNECTED_message,
	SOFTAP_DIST_IP_message,
	SOFTAP_DISCONNECTED_message
};

#define ESP01_MESSAGE_LENGTH(ID) (strlen_P((PGM_P)pgm_read_ptr(&(esp01_message[ID]))))

class SoftwareSerial;

#define TX_BUFFER_SIZE 382
#define RX_BUFFER_SIZE 382

class Esp01 {
	private:
		SoftwareSerial* _p_esp01Serial;

		char _tx_buffer[TX_BUFFER_SIZE];
		char _rx_buffer[RX_BUFFER_SIZE];

		char* _rx = _rx_buffer;

	public:
		Esp01();
		~Esp01();

		int atCommand(AT_COMMAND_ID);
		int atCommand(AT_COMMAND_ID, const char*, ...);
		int atResponse(int (*)(char*, int), const bool = true);
		int atEmptyResponseHandler(char*, int);

		bool isMessage(ESP01_MESSAGE_ID message, const char* data);

		int dataReceive(int (*)(const int, char*, int));
		int dataSend(const int, const char*, int);
};

#endif /* ESP01_AT_H_ */
