#include <Adafruit_NeoPixel.h>

#define pinData 12
#define btBlue  14
#define btRed  4

int state_btBlue = 0;
int state_btRed = 0;
//int brightness = 0;
//int fadeAmount = 30;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, pinData, NEO_GRB + NEO_KHZ800);

void setup()
{
  pinMode(btBlue, INPUT);
  pinMode(btRed, INPUT);
  strip.begin();
  strip.setBrightness(30);
  strip.show();
  Serial.begin(9600);
}
void loop()
{
  state_btBlue = digitalRead(btBlue);
  state_btRed = digitalRead(btRed);

  if (state_btRed == HIGH && state_btBlue == LOW)
  {
    colorWipe(strip.Color(255, 0, 0), 100); // Red
    // if (brightness == 0 || brightness == 255)

    // fadeAmount = -fadeAmount;
    Serial.print("red");
    Serial.println();

  }
  else if (state_btBlue == HIGH && state_btRed == LOW )
  {
    colorWipe(strip.Color(0, 0, 255), 100); // Green
    //if (brightness == 0 || brightness == 255)
    Serial.print("Blue");
    Serial.println();

    //fadeAmount = -fadeAmount;
  }
  else
  {

    strip.show();
    colorWipe(strip.Color(255, 0, 0), 50); // Red
    colorWipe(strip.Color(0, 255, 0), 50); // Green
    colorWipe(strip.Color(0, 0, 255), 50); // Blue
    rainbow(16);
   // rainbowCycle(16);
  }

}
void colorWipe(unsigned int c, unsigned int wait)
{
  for (int i = 0; i < strip.numPixels(); i++) //numPixels = numLEDs
  {
    strip.setPixelColor(i, c); //set color for once LED
    strip.show();
    delay(wait);
  }
}

//rainbow led for all
void rainbow(unsigned int wait)
{
  int i, j;

  for (j = 0; j < 25; j++)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// rainbow circle led
void rainbowCycle(unsigned int wait)
{
  uint16_t i, j;

  for (j = 0; j < 25 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//change LED from green -> red -> blue -> green (full color)
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
