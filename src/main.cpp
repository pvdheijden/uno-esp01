#include <Arduino.h>
#include <esp01.h>

#include "r1-measurement.h"

void setup() {
  Serial.begin(115200);
  Serial.println("minipro-esp01");

  esp01Init();

  setupR1();
}

ESP01_STATE esp01_state = AT_COMMAND;

ESP01_STATE atResponseHandler(const char* response, size_t length) {
	ESP01_STATE new_state = SOFTAP_MODE_COMMAND;

	Serial.println(F("new state: SOFTAP_MODE_COMMAND"));
	return new_state;
}

ESP01_STATE staModeResponseHandler(const char* response, size_t length) {
	ESP01_STATE new_state = WIFI_CONNECT_COMMAND;

	Serial.println(F("new state: WIFI_CONNECT_COMMAND"));
	return new_state;
}

ESP01_STATE softapModeResponseHandler(const char* response, size_t length) {
	ESP01_STATE new_state = WIFI_CHECK_COMMAND;

	Serial.println(F("new state: WIFI_CHECK_COMMAND"));
	return new_state;
}

ESP01_STATE wifiConnectResponseHandler(const char* response, size_t length) {
	ESP01_STATE new_state = WIFI_CHECK_COMMAND;

	Serial.println(F("new state: WIFI_CHECK_COMMAND"));
	return new_state;
}

ESP01_STATE wifiCheckResponseHandler(const char* response, size_t length) {
	ESP01_STATE new_state = IP_MUX_ON_COMMAND;

	Serial.println(F("new state: IP_MUX_ON_COMMAND"));
	return new_state;
}

ESP01_STATE ipMuxOnResponseHandler(const char* response, size_t length) {
	ESP01_STATE new_state = IP_SERVER_ON_COMMAND;

	Serial.println(F("new state: IP_SERVER_ON_COMMAND"));
	return new_state;
}

ESP01_STATE ipServerOnResponseHandler(const char* response, size_t length) {
	ESP01_STATE new_state = LISTEN;

	Serial.println(F("new state: LISTEN"));
	return new_state;
}

ESP01_STATE listenDataHandler(const char* data, size_t length) {
	ESP01_STATE new_state = LISTEN;

	Serial.println(F("new state: LISTEN"));
	return new_state;
}

void loop() {
  switch(esp01_state) {

    case AT_COMMAND:
      esp01_state = esp01Command(AT) != -1 ? AT_RESPONSE : -1;
      break;

    case AT_RESPONSE:
      esp01_state = esp01Response(AT_RESPONSE, &atResponseHandler);
      break;

    case STA_MODE_COMMAND:
      esp01_state = esp01Command(STA_MODE) != -1 ? STA_MODE_RESPONSE : -1;
      break;

    case STA_MODE_RESPONSE:
      esp01_state = esp01Response(STA_MODE_RESPONSE, &softapModeResponseHandler);
      break;

    case SOFTAP_MODE_COMMAND:
      esp01_state = esp01Command(SOFTAP_MODE) != -1 ? SOFTAP_MODE_RESPONSE : -1;
      break;

    case SOFTAP_MODE_RESPONSE:
      esp01_state = esp01Response(SOFTAP_MODE_RESPONSE, &softapModeResponseHandler);
      break;

    case WIFI_CHECK_COMMAND:
      esp01_state = esp01Command(WIFI_CHECK) != -1 ? WIFI_CHECK_RESPONSE : -1;
      break;

    case WIFI_CHECK_RESPONSE:
      esp01_state = esp01Response(WIFI_CHECK_RESPONSE, &wifiCheckResponseHandler);
      break;

    case WIFI_CONNECT_COMMAND:
      esp01_state = esp01Command(WIFI_CONNECT) != -1 ? WIFI_CONNECT_RESPONSE : -1;
      break;

    case WIFI_CONNECT_RESPONSE:
      esp01_state = esp01Response(WIFI_CONNECT_RESPONSE, &wifiConnectResponseHandler);
      break;

    case IP_MUX_ON_COMMAND:
      esp01_state = esp01Command(IP_MUX_ON) != -1 ? IP_MUX_ON_RESPONSE : -1;
      break;

    case IP_MUX_ON_RESPONSE:
      esp01_state = esp01Response(IP_MUX_ON_RESPONSE, &ipMuxOnResponseHandler);
      break;

    case IP_SERVER_ON_COMMAND:
      esp01_state = esp01Command(IP_SERVER_ON) != -1 ? IP_SERVER_ON_RESPONSE : -1;
      break;

    case IP_SERVER_ON_RESPONSE:
      esp01_state = esp01Response(IP_SERVER_ON_RESPONSE, &ipServerOnResponseHandler);
      break;

    case LISTEN:
    	esp01_state = esp01Listen(&listenDataHandler);
      break;

  }

  /**
   * Read resistance value 
   */
  updateR1();
}
