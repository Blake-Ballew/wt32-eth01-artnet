#include <Arduino.h>
#include <FastLED.h>
#include <ArtnetETH.h>

#define NUM_LEDS_PER_STRIP 144 // Modulight V3 Spec
#define NUMSTRIPS 12 // 12 per controller
#define NUMSTRIPS_PER_PIN 1
#define NB_CHANNEL_PER_LED 3 //Should be 4 if your sending tool is sending RGBW
#define UNIVERSE_SIZE_IN_CHANNEL  (NUM_LEDS_PER_STRIP * 3)  //here we define a universe of 170 pixels each pixel is composed of 3 channels
#define START_UNIVERSE 0 // Prefered for MadMapper

#define NUMBER_OF_LEDS 144 * NUMSTRIPS_PER_PIN
#define NB_LEDS_PER_UNIVERSE 144

ArtnetReceiver artnet;

CRGB *ledsPin14;
CRGB *ledsPin15;



void artnetCallback(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote)
{

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

  artnet.begin();

  artnet.forwardArtDmxDataToFastLED(1, ledsPin14, NUMBER_OF_LEDS);
  artnet.forwardArtDmxDataToFastLED(2, ledsPin15, NUMBER_OF_LEDS);
}

void loop() {
  artnet.parse(); // check if artnet packet has come and execute callback
  FastLED.show();
}
