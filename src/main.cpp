#include <Arduino.h>
#include <FastLED.h>
#include <ETH.h>

#define NUM_LEDS_PER_STRIP 144 // Modulight V3 Spec
#define NUMSTRIPS 12 // 12 per controller
#define NB_CHANNEL_PER_LED 3 //Should be 4 if your sending tool is sending RGBW
#define UNIVERSE_SIZE_IN_CHANNEL  (170 * 3)  //here we define a universe of 170 pixels each pixel is composed of 3 channels
#define START_UNIVERSE 0 // Prefered for MadMapper

#define NUMBER_OF_LEDS 144 * 6
#define NB_LEDS_PER_UNIVERSE 170
#define START_UNIVERSE 1

#include "artnetESP32V2.h"

artnetESP32V2 artnet;

CRGB *ledsPin14;
CRGB *ledsPin15;

// I2SClocklessLedDriver driver;
// int pins[]={14};

void artnetCallbackPin14(void *param)
{
  subArtnet *subartnet = (subArtnet *)param;
  auto dataLen = subartnet->len;

  for (int i = 0; i < dataLen; i += NB_CHANNEL_PER_LED)
  {
    size_t ledIdx = i / NB_CHANNEL_PER_LED;
    ledsPin14[ledIdx] = CRGB(subartnet->data[i], subartnet->data[i + 1], subartnet->data[i + 2]);
  }

  FastLED.show();
}

void artnetCallbackPin15(void *param)
{
  subArtnet *subartnet = (subArtnet *)param;
  auto dataLen = subartnet->len;

  for (int i = 0; i < dataLen; i += NB_CHANNEL_PER_LED)
  {
    size_t ledIdx = i / NB_CHANNEL_PER_LED;
    ledsPin15[ledIdx] = CRGB(subartnet->data[i], subartnet->data[i + 1], subartnet->data[i + 2]);
  }

  FastLED.show();
}

void setup() {
  Serial.begin(115200);

  ledsPin14 = new CRGB[NUMBER_OF_LEDS];
  ledsPin15 = new CRGB[NUMBER_OF_LEDS];
  FastLED.addLeds<WS2812B, 14, GRB>(ledsPin14, NUMBER_OF_LEDS);
  FastLED.addLeds<WS2812B, 15, GRB>(ledsPin14, NUMBER_OF_LEDS);

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

  // reciever size (NUM_LEDS * 3), NUM_LEDS = 144*6
  uint32_t data_size = UNIVERSE_SIZE_IN_CHANNEL * 6;

  // Universes 1-6 go to GPIO pin 14
  artnet.addSubArtnet(1, data_size, UNIVERSE_SIZE_IN_CHANNEL ,&artnetCallbackPin14);

  // Universes 7 - 12 go to GPIO pin 15
  artnet.addSubArtnet(7, data_size, UNIVERSE_SIZE_IN_CHANNEL ,&artnetCallbackPin15);



  artnet.setNodeName("SIGMA v4.20 OBAMA SIGNATURE EDITION (brain smoothed firmware v666)");

  if (artnet.listen(6454)) 
  {
      Serial.print("artnet Listening on IP: ");
      Serial.println(ETH.localIP()); 
  }
}

void loop() {
  vTaskDelete(NULL);
}
