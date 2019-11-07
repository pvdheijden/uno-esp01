/*
 * at-command.h
 *
 *  Created on: Nov 2, 2019
 *      Author: Pim
 */

#ifndef ESP01_ESP01_H_
#define ESP01_ESP01_H_

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
	IP_SERVER_ON = 8
} AT_COMMAND_ID;

static const char AT_command[] PROGMEM = "AT\r\n";
static const char RESTART_command[] PROGMEM = "AT+RST\r\n";
static const char VERSION_command[] PROGMEM = "AT+GMR\r\n";
static const char STA_MODE_command[] PROGMEM = "AT+CWMODE=1\r\n";
static const char SOFTAP_MODE_command[] PROGMEM = "AT+CWMODE=3\r\n";
static const char WIFI_CHECK_command[] PROGMEM = "AT+CIFSR\r\n";
static const char WIFI_CONNECT_command[] PROGMEM = "AT+CWJAP=\"Spoetnik\",\"smilingowl440\"\r\n";
static const char IP_MUX_ON_command[] PROGMEM = "AT+CIPMUX=1\r\n";
static const char IP_SERVER_ON_command[] PROGMEM = "AT+CIPSERVER=1,3333\r\n";

PGM_P const at_commands[] PROGMEM = {
	AT_command,
	RESTART_command,
	VERSION_command,
	STA_MODE_command,
	SOFTAP_MODE_command,
	WIFI_CHECK_command,
	WIFI_CONNECT_command,
	IP_MUX_ON_command,
	IP_SERVER_ON_command
};

size_t const at_command_lengths[] = {
	strlen(AT_command),
	strlen(RESTART_command),
	strlen(VERSION_command),
	strlen(STA_MODE_command),
	strlen(SOFTAP_MODE_command),
	strlen(WIFI_CHECK_command),
	strlen(WIFI_CONNECT_command),
	strlen(IP_MUX_ON_command),
	strlen(IP_SERVER_ON_command)
};

typedef enum _RESPONSE_STATUS {
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
	SOFTAP_DIST_STA_IP = 12,
	SOFTAP_STA_DISCONNECTED = 13
} RESPONSE_STATUS;

static const char OK_status[] PROGMEM = "OK\r\n";
static const char READY_status[] PROGMEM = "ready\r\n";
static const char ERROR_status[] PROGMEM = "ERROR\r\n";
static const char WIFI_CONNECTED_status[] PROGMEM = "WIFI CONNECTED\r\n";
static const char WIFI_GOT_IP_status[] PROGMEM = "WIFI GOT IP\r\n";
static const char WIFI_DISCONNECT_status[] PROGMEM = "WIFI DISCONNECT\r\n";
static const char BUSY_S_status[] PROGMEM = "busy s...\r\n";
static const char BUSY_P_status[] PROGMEM = "busy p...\r\n";
static const char NETWORK_CONNECT_status[] PROGMEM = ",CONNECT\r\n";
static const char NETWORK_CLOSED_status[] PROGMEM = ",CLOSED\r\n";
static const char NETWORK_IPD_status[] PROGMEM = "+IPD\r\n";
static const char SOFTAP_CONNECTED_status[] PROGMEM = "+STA_CONNECTED:\r\n";
static const char SOFTAP_DIST_STA_IP_status[] PROGMEM = "+DIST_STA_IP:\r\n";
static const char SOFTAP_DISCONNECTED_status[] PROGMEM = "+STA_DISCONNECTED:\r\n";

PGM_P const response_status[] PROGMEM = {
	OK_status,
	READY_status,
	ERROR_status,
	WIFI_CONNECTED_status,
	WIFI_GOT_IP_status,
	WIFI_DISCONNECT_status,
	BUSY_S_status,
	BUSY_P_status,
	NETWORK_CONNECT_status,
	NETWORK_CLOSED_status,
	NETWORK_IPD_status,
	SOFTAP_CONNECTED_status,
	SOFTAP_DIST_STA_IP_status,
	SOFTAP_DISCONNECTED_status
};

size_t const response_status_lengths[] = {
	strlen(OK_status),
	strlen(READY_status),
	strlen(ERROR_status),
	strlen(WIFI_CONNECTED_status),
	strlen(WIFI_GOT_IP_status),
	strlen(WIFI_DISCONNECT_status),
	strlen(BUSY_S_status),
	strlen(BUSY_P_status),
	strlen(NETWORK_CONNECT_status),
	strlen(NETWORK_CLOSED_status),
	strlen(NETWORK_IPD_status),
	strlen(SOFTAP_CONNECTED_status),
	strlen(SOFTAP_DIST_STA_IP_status),
	strlen(SOFTAP_DISCONNECTED_status)
};

inline bool isResponseStatus(RESPONSE_STATUS status, const char* response) {
	return strncmp_P(response, (PGM_P)pgm_read_word(&(response_status[status])), response_status_lengths[status] ) == 0 ? true : false;
}

/**
 * Command and response states
 */
typedef enum _ESP01_STATE {
	LISTEN,
	AT_COMMAND,
	AT_RESPONSE,
	VERSION_COMMAND,
	VERSION_RESPONSE,
	STA_MODE_COMMAND,
	STA_MODE_RESPONSE,
	SOFTAP_MODE_COMMAND,
	SOFTAP_MODE_RESPONSE,
	WIFI_CHECK_COMMAND,
	WIFI_CHECK_RESPONSE,
	WIFI_CONNECT_COMMAND,
	WIFI_CONNECT_RESPONSE,
	IP_MUX_ON_COMMAND,
	IP_MUX_ON_RESPONSE,
	IP_SERVER_ON_COMMAND,
	IP_SERVER_ON_RESPONSE,

} ESP01_STATE;

void esp01Init();
size_t esp01Command(AT_COMMAND_ID);
ESP01_STATE esp01Response(ESP01_STATE, ESP01_STATE (*)(const char*, const size_t));
ESP01_STATE esp01Listen(ESP01_STATE (*)(const char*, const size_t));

#endif /* ESP01_ESP01_H_ */
