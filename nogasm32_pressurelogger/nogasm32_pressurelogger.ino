// OTA Stuff
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FastLED.h>

#define NUM_LEDS 24
#define DATA_PIN 16
CRGB leds[NUM_LEDS];
int dot = 0;
int dir = 1;

const char* ssid = "esp-update-net";
const char* password = "supersavepw";
const char* hostname = "nogasm32-pressurelogger";
bool otaActive = false;

//Encoder
#define ENC_SW   13 //Pushbutton on the encoder
//Pressure Sensor Analog In
#define BUTTPIN 12
#define FREQUENCY 120
#define LED_FREQUENCY 10
//Update/render period
#define period (1000/FREQUENCY)
#define ledPeriod (1000/LED_FREQUENCY)

long lastUpdate = 0;
long lastLedCycle = 0;

void setup() {
  // Wait for boot
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  for(int dot = 0; dot < NUM_LEDS; dot++)
  {
    leds[dot] = CRGB::Gold;
    FastLED.show();
    delay(100);
    leds[dot] = CRGB::Black;
  }
  for(int dot = NUM_LEDS-1; dot >= 0; dot--)
  {
    leds[dot] = CRGB::Gold;
    FastLED.show();
    delay(100);
    leds[dot] = CRGB::Black;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  otaActive = (WiFi.waitForConnectResult() == WL_CONNECTED);
  // Setup OTA only if WiFi connected
  if(otaActive)
  {
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
    });
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.begin();
    for(int i = 0; i < (NUM_LEDS>>1); i++)
    {
      leds[i<<1] = CRGB::DarkGreen;
    }
    FastLED.show();
  }
  else
  {
    pinMode(ENC_SW,   INPUT); //Pin to read when encoder is pressed
    digitalWrite(ENC_SW, HIGH); // Encoder switch pullup
    pinMode(BUTTPIN,INPUT); //default is 12 bit resolution (4096), 0-3.3

    Serial.begin(115200);
  }
}

//=======Main Loop=============================
void loop() {
  if(otaActive)
  {
    if(!digitalRead(ENC_SW)) otaActive = false;
    ArduinoOTA.handle();
  }
  else
  {
    if (millis() - period > lastUpdate) {      
      long pressure = analogRead(BUTTPIN);
      //Report pressure and motor data over USB for analysis / other uses. timestamps disabled by default
      Serial.print(millis()); //Timestamp (ms)
      Serial.print(",");
      //Serial.print(motSpeed); //Motor speed (0-255)
      //Serial.print(",");
      Serial.print(pressure); //(Original ADC value - 12 bits, 0-4095)
      Serial.print(",");
      Serial.println(digitalRead(ENC_SW));
      //Serial.println(constrain(100 * arousal/ (float)pLimit, 0, 100));
      lastUpdate = millis();
    }
    if(millis() - ledPeriod > lastLedCycle) 
    {
      leds[dot] = CRGB::DarkRed;
      FastLED.show();
      leds[dot] = CRGB::Black;
      dot = dot + dir;
      if(dot >= NUM_LEDS - 1) dir = -1;
      if(dot <= 0) dir = 1; 
      lastLedCycle = millis();
    }
  }
}
