#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include <time.h>
#include <ArduinoOTA.h>
#include <ESP8266MQTTClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

#include "SSD1306.h"

//driver 1
#define in11    3
#define in12    1
#define in13    16
#define in14    0
#define ena11   15
#define ena12   13

//driver 2
#define in21    2
#define in22    10
#define in23    12
#define in24    14

#define directspd 512
#define rotatespd 600

#define PIN_LED 16
char var_data[10] = "";
int length_old = 0;
int length_new = 0;
bool state = 0;
int cnt = 0;
int mode_ctl = 0;
int cnt_stop = 0;

SSD1306  display(0x3c, 4, 5);

#define OTA_PASS "iotmaker"
#define OTA_PORT (8266)

const char ApiKey[] = "H1g9ivZj1-";
const char ApiURL[] = "http://fota.vn";
const char AppVersion[] = "1.0";
MQTTClient mqtt;
String devId = "";

Ticker ticker;

void tick()
{
  int state = digitalRead(PIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(PIN_LED, !state);     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.5, tick);
}

void  rightRearWheelGo()
{
  digitalWrite(in11, HIGH);
  digitalWrite(in12, LOW);
}

void  rightRearWheelBack()
{
  digitalWrite(in11, LOW);
  digitalWrite(in12, HIGH);
}

void  leftRearWheelGo()
{
  digitalWrite(in13, HIGH);
  digitalWrite(in14, LOW);
}

void  leftRearWheelBack()
{
  digitalWrite(in13, LOW);
  digitalWrite(in14, HIGH);
}

void  rightFrontWheelGo()
{
  digitalWrite(in21, HIGH);
  digitalWrite(in22, LOW);
}

void  rightFrontWheelBack()
{
  digitalWrite(in21, LOW);
  digitalWrite(in22, HIGH);
}

void  leftFrontWheelGo()
{
  digitalWrite(in23, HIGH);
  digitalWrite(in24, LOW);
}

void  leftFrontWheelBack()
{
  digitalWrite(in23, LOW);
  digitalWrite(in24, HIGH);
}

void goAway()
{
  leftRearWheelGo();
  rightRearWheelGo();
  leftFrontWheelGo();
  rightFrontWheelGo();
  analogWrite(ena11, directspd);
  analogWrite(ena12, directspd);
  display.clear();
  display.drawString(40, 30, "Go");
  display.display();
}

void back()
{
  leftRearWheelBack();
  rightRearWheelBack();
  leftFrontWheelBack();
  rightFrontWheelBack();
  analogWrite(ena11, directspd);
  analogWrite(ena12, directspd);
  display.clear();
  display.drawString(40, 30, "Back");
  display.display();
}

void turnLeft()
{
  leftRearWheelBack();
  rightRearWheelGo();
  leftFrontWheelBack();
  rightFrontWheelGo();
  analogWrite(ena12, rotatespd);
  analogWrite(ena11, rotatespd);
  display.clear();
  display.drawString(40, 30, "Left");
  display.display();
}

void turnRight()
{
  leftRearWheelGo();
  rightRearWheelBack();
  leftFrontWheelGo();
  rightFrontWheelBack();
  analogWrite(ena11, rotatespd);
  analogWrite(ena12, rotatespd);
  display.clear();
  display.drawString(40, 30, "Right");
  display.display();
}

void Stop()
{
  digitalWrite(in11, LOW);
  digitalWrite(in12, LOW);
  digitalWrite(in13, LOW);
  digitalWrite(in14, LOW);
  digitalWrite(in21, LOW);
  digitalWrite(in22, LOW);
  digitalWrite(in23, LOW);
  digitalWrite(in24, LOW);
  display.clear();
  display.drawString(40, 30, "Stop");
  display.display();
}


void setup() {
  WiFiManager wifiManager;
  HTTPClient http;
  Serial.begin(115200);
//  Serial.setDebugOutput(true);
  pinMode(PIN_LED, OUTPUT);

  pinMode(in11, OUTPUT);
  pinMode(in12, OUTPUT);
  pinMode(in13, OUTPUT);
  pinMode(in14, OUTPUT);
  pinMode(ena11, OUTPUT);
  pinMode(ena12, OUTPUT);

  pinMode(in21, OUTPUT);
  pinMode(in22, OUTPUT);
  pinMode(in23, OUTPUT);
  pinMode(in24, OUTPUT);

  analogWrite(ena11, 0);
  analogWrite(ena12, 0);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.drawString(40, 30, "Stop");
  display.display();

  devId = WiFi.macAddress();

  delay(500);
  Serial.println();
  Serial.println("+---------------------------------+");
  Serial.println("| APP Version =  " + String(AppVersion) + "              |");
  Serial.println("| DeviceId =  " + devId + "   |");
  Serial.println("| API Key =  " + String(ApiKey) + "            |");
  Serial.println("+---------------------------------+");

  ticker.attach(1, tick);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);


  if (!wifiManager.autoConnect(("IOTMAKER_" + String(ESP.getChipId())).c_str())) {
    ESP.reset();
    delay(1000);
  }
  //LED_ON();
  ticker.detach();

  //Connected, config timer, register device
  //  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  String url = "http://fota.vn/api/device/" + String(ApiKey) + "?id=" + devId;
  http.begin(url);

  Serial.println("Register device: " + url);
  Serial.println("Status code = " + String(http.GET()));

  url = "http://fota.vn/api/fota/" + String(ApiKey);

  Serial.println("Checking update, url=" + String(url));

  t_httpUpdate_return ret = ESPhttpUpdate.update(url, String(AppVersion));

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      ESP.restart();
      break;
  }

  url = "mqtt://" + devId + ":" + String(ApiKey) + "@fota.vn";
  Serial.println("Begin MQTT=" + url);

  mqtt.onData([](String topic, String data, bool cont) {
    char var[1] = "";
    StaticJsonBuffer<200> jsonBuffer;
    Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
    JsonObject& root = jsonBuffer.parseObject(data);
    length_new = data.length();
    
    for (int i = 0; i < length_old; i++)
    {
      var_data[i] = var[0];
    }
    
    for (int i = 1; i < (length_new - 1); i++)
    {
      var_data[i - 1] = data[i];
    }

    if (String(var_data) == "auto")
    {
      state = 1;
    }
    else
    {
      state = 0;
    }

    length_old = length_new;

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }

  });

  mqtt.onSubscribe([](int sub_id) {
    Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
  });
  mqtt.onConnect([]() {
    Serial.printf("MQTT: Connected\r\n");
    mqtt.subscribe("/dev/" + devId + "/sub", 0);
  });

  mqtt.begin(url);

  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA.begin();

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
    mqtt.handle();
  }

  if (state == 0)
  {
    cnt = 0;
    mode_ctl = 0;
    cnt_stop = 0;
    if (String(var_data) == "up")
    {
      goAway();
    }
    else if (String(var_data) == "down")
    {
      back();
    }
    else if (String(var_data) == "stop")
    {
      Stop();
    }
    else if (String(var_data) == "left")
    {
      turnLeft();
    }
    else if (String(var_data) == "right")
    {
      turnRight();
    }
  }
  else if (state == 1)
  {
    if (mode_ctl == 0)
    {
      goAway();
      cnt++;
    }
    else if (mode_ctl == 1)
    {
      Stop();
      cnt_stop++;
    }
    else if (mode_ctl == 2)
    {
      back();
      cnt++;
    }
    else if (mode_ctl == 3)
    {
      Stop();
      cnt_stop++;
    }
    else if (mode_ctl == 4)
    {
      turnLeft();
      cnt++;
    }
    else if (mode_ctl == 5)
    {
      Stop();
      cnt_stop++;
    }
    else if (mode_ctl == 6)
    {
      turnRight();
      cnt++;
    }
    else if (mode_ctl == 7)
    {
      Stop();
      cnt_stop++;
    }
    if (cnt > 700)
    {
      cnt = 0;;
      mode_ctl++;
    }
    if (cnt_stop > 200)
    {
      cnt_stop = 0;
      mode_ctl++;
    }
    if (mode_ctl > 7)
    {
      mode_ctl = 0;
    }
  }
}
