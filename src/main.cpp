#include <Arduino.h>
#include "I2SClocklessLedDriver.h"
#include <ETH.h>

#define NUMBER_OF_LEDS 170
#define NB_LEDS_PER_UNIVERSE 170
#define START_UNIVERSE 1
#define NB_CHANNEL_PER_LED 3
#define UNIVERSE_SIZE_IN_CHANNEL  (170 * 3)  //here we define a universe of 170 pixels each pixel is composed of 3 channels

#include "artnetESP32V2.h"

artnetESP32V2 artnet;

I2SClocklessLedDriver driver;
int pins[]={14};

void artnetCallback(void *param)
{
  subArtnet *subartnet = (subArtnet *)param;
  driver.showPixels(NO_WAIT,subartnet->data);
}

void setup() {
  Serial.begin(115200);

  driver.initled(NULL,pins,NUMSTRIPS,NUM_LEDS_PER_STRIP);  
  driver.setBrightness(20);

  if (!ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720))
  {
    Serial.println("Failed to start ETH");
    while (true) {
      delay(1000);
    }
  }

  artnet.addSubArtnet(START_UNIVERSE ,NUM_LEDS_PER_STRIP * NUMSTRIPS * NB_CHANNEL_PER_LED,UNIVERSE_SIZE_IN_CHANNEL ,&artnetCallback);
  artnet.setNodeName("Arnet Node esp32");

  if (artnet.listen(6454)) 
  {
      Serial.print("artnet Listening on IP: ");
      Serial.println(WiFi.localIP());
  }
}

void loop() {
  vTaskDelete(NULL);
}
