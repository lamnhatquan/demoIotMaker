#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include "SSD1306.h"

Adafruit_BMP280 bmp; // I2C

SSD1306  display(0x3c, 4, 5);

void setup()
{
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(15, 20, "IotMaker.VN");
  display.display();
  delay(2000);

  if (!bmp.begin()) {
    display.clear();
    display.drawString(0, 20, "Not Connect BMP");
    display.display();
    while (1) {
      delay(1);
    }
  }
}

void loop()
{
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Temperature: " + String(bmp.readTemperature()) + "*C");
  display.drawString(0, 20, "Pressure: " + String(bmp.readPressure()) + "Pa");
  display.drawString(0, 40, "Approx Altitude: " + String(bmp.readAltitude()) + "m");
  display.display();
  delay(2000);
}
