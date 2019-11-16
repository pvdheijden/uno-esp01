#include <Arduino.h>
#include <esp01.h>
class Esp01 esp01 = Esp01();

#include "r1-measurement.h"
class R1Measurement r1_measurement = R1Measurement();


int dataReceiveHandler(const int link, const char* data, int length) {
  if (strncmp("R1", data, length) == 0) {
    String R1 = String(r1_measurement.get(), 2);
    return esp01.dataSend(link, R1.c_str(), R1.length());
  }

	return 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("uno-esp01");

  //esp01 = new Esp01();

  esp01.atCommand(AT);
  esp01.atResponse(NULL);

/*
  esp01->atCommand(SOFTAP_MODE);
  esp01->atResponse(NULL);

  esp01->atCommand(IP_MUX_ON);
  esp01->atResponse(NULL);

  esp01->atCommand(IP_SERVER_ON);
  esp01->atResponse(NULL);

  esp01->atCommand(WIFI_CONNECT);
  esp01->atResponse(NULL);
*/

  esp01.atCommand(STA_MODE);
  esp01.atResponse(NULL);

  esp01.atCommand(WIFI_CHECK);
  esp01.atResponse(NULL);
}

void loop() {
  esp01.dataReceive(&dataReceiveHandler);

  /**
   * Read resistance value 
   */
  r1_measurement.update();
}
