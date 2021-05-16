#include <Arduino.h>
#include <FastLED.h>

#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const int NUM_LEDS = 3;
const int LED_PIN = 27;
#define COLOR_ORDER GRB
#define CURRENT_LIMIT 2000
#define BRIGHTNESS 255
#define LED_ID_PARAM "id"
#define COLOR_R_PARAM "r"
#define COLOR_G_PARAM "g"
#define COLOR_B_PARAM "b"
#define MULTI_LINE_STRING(a) #a
CRGB leds[NUM_LEDS];
typedef enum MODE {
  COLOR = 1,
  AMIM = 2
};

MODE curMode = MODE::COLOR;
int animId = 0;
int x=rand()%256,y=rand()%256,z=rand()%256;
int s = 1;

const char* ssid = "zza-net";  //replace
const char* password =  "@@@@@@@"; //replace

AsyncWebServer server(80);
extern const char index_html[] asm("_binary_src_rgb_html_start");

void fillAll(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}


void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0) 
    FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  fillAll(CRGB(0,0,0));
  FastLED.show();
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/anim", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain","ok");
    animId = -1;
    if (request->hasParam("id")) {
      animId = request->getParam("id")->value().toInt();
    }
    if (request->hasParam("s")) {
      s = request->getParam("s")->value().toInt();
    }
    if (animId < 0) animId = 0;
    if (s <= 0) s = 1;
    curMode = MODE::AMIM;
  });

  server.on("/led", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain","ok");
    String id = "-1";
    String colorR = "0";
    String colorG = "0";
    String colorB = "0";
    curMode = MODE::COLOR;
    if (request->hasParam(LED_ID_PARAM)) {
      id = request->getParam(LED_ID_PARAM)->value();
    }
    if (request->hasParam(COLOR_R_PARAM)) {
      colorR = request->getParam(COLOR_R_PARAM)->value();
    }
    if (request->hasParam(COLOR_G_PARAM)) {
      colorG = request->getParam(COLOR_G_PARAM)->value();
    }
    if (request->hasParam(COLOR_B_PARAM)) {
      colorB = request->getParam(COLOR_B_PARAM)->value();
    }
    CRGB color = CRGB(colorR.toInt(),colorG.toInt(),colorB.toInt());
    if (id.toInt() == -1)
    {
      fillAll(color);
    }
    else
    {
      leds[id.toInt()%NUM_LEDS] = color;
    }
    //FastLED.show();
  });
  
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  EVERY_N_MILLIS(25) {
    if (curMode == MODE::AMIM)
    {
      switch (animId)
      {
      case 0:
        leds[0].setHue((x+=s)%256);
        leds[1].setHue((y+=s)%256);
        leds[2].setHue((z+=s)%256);
        break;
      }
    }
    FastLED.show();
  }
}