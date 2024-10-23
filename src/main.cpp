#include <Arduino.h>
#include <FastLED.h>
#include <ETH.h>

#define NUMBER_OF_LEDS 170
#define NB_LEDS_PER_UNIVERSE 170
#define START_UNIVERSE 1
#define NB_CHANNEL_PER_LED 3
#define UNIVERSE_SIZE_IN_CHANNEL  (170 * 3)  //here we define a universe of 170 pixels each pixel is composed of 3 channels

#include "artnetESP32V2.h"

artnetESP32V2 artnet;

CRGB *leds;

// I2SClocklessLedDriver driver;
// int pins[]={14};

void artnetCallback(void *param)
{
  subArtnet *subartnet = (subArtnet *)param;
  auto dataLen = subartnet->len;

  for (int i = 0; i < dataLen; i += NB_CHANNEL_PER_LED)
  {
    size_t ledIdx = i / NB_CHANNEL_PER_LED;
    leds[ledIdx] = CRGB(subartnet->data[i], subartnet->data[i + 1], subartnet->data[i + 2]);
  }

  FastLED.show();
}

void setup() {
  Serial.begin(115200);

  leds = new CRGB[NUMBER_OF_LEDS];
  FastLED.addLeds<WS2812B, 14, GRB>(leds, NUMBER_OF_LEDS);

  if (!ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720))
  {
    Serial.println("Failed to start ETH");
    while (true) 
    {
      delay(10000);
      ESP.restart();
    }
  }

  ETH.config(IPAddress(192, 168, 1, 10), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  ETH.setHostname("obelisk-control-board-1");

  artnet.addSubArtnet(START_UNIVERSE, NUMBER_OF_LEDS * NB_CHANNEL_PER_LED,UNIVERSE_SIZE_IN_CHANNEL ,&artnetCallback);
  artnet.setNodeName("Arnet Node esp32");

  if (artnet.listen(6454)) 
  {
      Serial.print("artnet Listening on IP: ");
      Serial.println(ETH.localIP()); 
  }
}

void loop() {
  vTaskDelete(NULL);
}
