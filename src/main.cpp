#include <Arduino.h>
#include <esp01.h>

#include "r1-measurement.h"

ESP01_STATE receiveDataHandler(const int link, const char* data, int length) {
  if (strncmp("R1", data, length) == 0) {
    String R1 = String(getR1(), 2);
    esp01Send(link, R1.c_str(), R1.length());
  }

	return RECEIVE;
}

ESP01_STATE esp01_state = AT_COMMAND;

void setup() {
  Serial.begin(115200);
  Serial.println("uno-esp01");

  esp01Init();

  setupR1();
}

void loop() {
  switch(esp01_state) {

    case RESTART_COMMAND:
      if (esp01Command(RESTART, NULL) != -1) {
        esp01_state = RESTART_RESPONSE;
      }
      break;

    case RESTART_RESPONSE:
      esp01_state = esp01Response(RESTART_RESPONSE, AT_COMMAND);
      break;

    case AT_COMMAND:
      if (esp01Command(AT, NULL) != -1) {
        esp01_state = AT_RESPONSE;
      }
      break;

    case AT_RESPONSE:
      esp01_state = esp01Response(AT_RESPONSE, SOFTAP_MODE_COMMAND);
      break;

    case STA_MODE_COMMAND:
      if (esp01Command(STA_MODE, NULL) != -1) {
        esp01_state = STA_MODE_RESPONSE;
      }
      break;

    case STA_MODE_RESPONSE:
      esp01_state = esp01Response(STA_MODE_RESPONSE, WIFI_CHECK_COMMAND);
      break;

    case SOFTAP_MODE_COMMAND:
      if (esp01Command(SOFTAP_MODE, NULL) != -1) {
        esp01_state = SOFTAP_MODE_RESPONSE;
      }
      break;

    case SOFTAP_MODE_RESPONSE:
      esp01_state = esp01Response(SOFTAP_MODE_RESPONSE, IP_MUX_ON_COMMAND);
      break;

    case IP_MUX_ON_COMMAND:
      if (esp01Command(IP_MUX_ON, NULL) != -1) {
        esp01_state = IP_MUX_ON_RESPONSE;
      }
      break;

    case IP_MUX_ON_RESPONSE:
      esp01_state = esp01Response(IP_MUX_ON_RESPONSE, IP_SERVER_ON_COMMAND);
      break;

    case IP_SERVER_ON_COMMAND:
      if (esp01Command(IP_SERVER_ON, NULL) != -1) {
        esp01_state = IP_SERVER_ON_RESPONSE;
      }
      break;

    case IP_SERVER_ON_RESPONSE:
      esp01_state = esp01Response(IP_SERVER_ON_RESPONSE, WIFI_CONNECT_COMMAND);
      break;

    case WIFI_CONNECT_COMMAND:
      if (esp01Command(WIFI_CONNECT, NULL) != -1) {
        esp01_state = WIFI_CONNECT_RESPONSE;
      }
      break;

    case WIFI_CONNECT_RESPONSE:
      esp01_state = esp01Response(WIFI_CONNECT_RESPONSE, WIFI_CHECK_COMMAND);
      break;

    case WIFI_CHECK_COMMAND:
      if (esp01Command(WIFI_CHECK, NULL) != -1) {
        esp01_state = WIFI_CHECK_RESPONSE;
      }
      break;

    case WIFI_CHECK_RESPONSE:
      esp01_state = esp01Response(WIFI_CHECK_RESPONSE, RECEIVE);
      break;

    case RECEIVE:
    	esp01_state = esp01Receive(&receiveDataHandler);
      break;

  }

  /**
   * Read resistance value 
   */
  updateR1();
}
