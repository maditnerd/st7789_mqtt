/*
  IFTT / Adafruit IO TFT Display
  Author : Remi Sarrailh (@m4dnerd)
  Licence : MIT
*/

#include "arduino_secrets.h" //Save credentials inside sketch

//#include <arduino_secrets.h> // Save credentials globally
// Create a folder in arduino/library/arduino_secrets and copy arduino_secrets.h in it
// More information on Andreas Spiess Channel -- https://www.youtube.com/watch?v=CAGQ8h8PKX4&t=347s

// TFT Screen
#include <TFT_eSPI.h> // Don't forget to change User_Setup.h inside TFT_eSPI library !
#include "bitmaps.h"  // (Bitmaps generated with LCD Image Converter) https://sourceforge.net/projects/lcd-image-converter/
// The custom font file attached to this sketch must be included
#include "arial12pt8b.h"
#define MYFONT &arial12pt8b
TFT_eSPI tft = TFT_eSPI();

// MQTT Subscriptions
#include "connection.h" // Connection / MQTT manager
#define FEED_NOTIFICATION "/feeds/notifications"
Adafruit_MQTT_Subscribe notification = Adafruit_MQTT_Subscribe(&mqtt, ADAFRUIT_MQTT_USERNAME FEED_NOTIFICATION);

void setup(void) {
  Serial.begin(115200);

  tft.begin();     // initialize a ST7789 chip
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.fillScreen(TFT_BLACK); //Fill screen with black
  tft.setFreeFont(MYFONT);

  //Display Wifi image / Wifi SSID name
  tft.pushImage(0,0,240,240,wifi);
  tft.drawString(HOME_SSID, 25, 20);

  wifiConnect(); //Wifi Connection
  mqttSetup();

  //Subscribe to feeds
  notification.setCallback(notificationCallBack);
  mqtt.subscribe(&notification);
}

//When message is sent to feed
void notificationCallBack(char *data, uint16_t len){
      tft.pushImage(0,0,240,240,notifications);
      Serial.println(data);
      tft.setCursor(0,50); //Set text cursor to 0,50
      tft.println(data); //Print text
}

void loop() {
  MQTT_connect(); //Maintain MQTT connection
  mqtt.processPackets(10000); //Check for message

  if(! mqtt.ping()) { //Check MQTT Connection
    mqtt.disconnect();
  }
}


void MQTT_connect() {
    int8_t ret;

    // Stop if already connected.
    if (mqtt.connected()) {
        return;
    }

    //Display Adafruit IO message
    tft.pushImage(0,0,240,240,adafruit_io);
    tft.drawString(ADAFRUIT_MQTT_USERNAME, 25, 20);

    Serial.print("Connecting to MQTT... ");
    uint8_t retries = 10;
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
            Serial.println(mqtt.connectErrorString(ret));
            Serial.println("Retrying MQTT connection in 5 seconds...");
            mqtt.disconnect();
            delay(5000);  // wait 5 seconds
            retries--;
            if (retries == 0) {
            // basically die and wait for WDT to reset me
                while(1);
            }
        }
    Serial.println("MQTT Connected!");

    //Display notification screen
    tft.pushImage(0,0,240,240,notifications);
}
