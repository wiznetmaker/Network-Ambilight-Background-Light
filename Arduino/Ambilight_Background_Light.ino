#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

// Define the number of WS2812 LEDs and the pin to which they are connected
#define NUM_LEDS 150
#define LED_PIN 28

// Create an instance of the Adafruit NeoPixel library
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

#include "TouchyTouch.h"
const int touch_threshold_adjust = 300;
const int touch_pins[] = {5};
const int touch_count = sizeof(touch_pins) / sizeof(int);
TouchyTouch touches[touch_count];
uint32_t time_now = 0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x00, 0x08, 0xdc, 0x11, 0x22, 0xED
};
IPAddress ip(10, 0, 1, 227);

// Create an instance of the Ethernet server
EthernetServer server(5000);

struct RGBColor {
  byte red;
  byte green;
  byte blue;
};

void setup() {
  for (int i = 0; i < touch_count; i++) {
    touches[i].begin( touch_pins[i] );
    touches[i].threshold += touch_threshold_adjust; // make a bit more noise-proof
  }  
  // Initialize components
  Serial.begin(115200);
//   while (!Serial) {
//     ; // wait for serial port to connect. Needed for native USB port only
//   }
//  delay(1000);
  Serial.println("POE WS2812.");
   Ethernet.init(17);  // WIZnet W5100S-EVB-Pico
   // start the Ethernet connection and the server:
   Ethernet.begin(mac, ip);
   if (Ethernet.linkStatus() == LinkOFF) {
     Serial.println("Ethernet cable is not connected.");
   }
   server.begin();
   strip.begin();
   strip.show();  // Initialize all pixels to 'off'
}

char color_char[444];
uint16_t color_num = 0;
void loop() {
  EthernetClient client = server.available();  
  if (client) {
    if (client.connected()) {
       while (client.available()) {
        char c = client.read();
        color_char[color_num] = c;
        color_num++;
        Serial.println(color_num);
       }
       if(color_num >= 444)
        {
          color_num =0;
          for (int i = 1; i < NUM_LEDS-1; i++) {
            strip.setPixelColor(i, strip.Color(color_char[i*3], color_char[i*3+1], color_char[i*3+2]));
          }
          strip.show();  // Update all pixels
        }
    }
  }
}

void Touch_handling()
{
  // key handling
  for ( int i = 0; i < touch_count; i++) {
    touches[i].update();
    if ( touches[i].rose() ) {
      Serial.print("Button:");
      Serial.println(i);
      time_now = millis();
    }
    if ( touches[i].fell() ) {
     Serial.printf("Release:");
     Serial.println(i);
     if((millis()-time_now)>500)
     {
        Serial.print("Time_enough");
     }
    }
  }
}
