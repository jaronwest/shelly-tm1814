// for controlling TM1814 LED strip (12v AWWRGB)
#include <NeoPixelBus.h>

void handleStrip() {
  for (int i = 0; i < pixelCount; i++)
  {
    // off
    if (!power) {
      strip.SetPixelColor(i, RgbwColor(0,0,0,0));
      digitalWrite(powerPin, LOW);
    }
    // single color
    else {
      digitalWrite(powerPin, HIGH);
      if (mode == 1) {
        strip.SetPixelColor(i, RgbwColor(ww, b, g, r));
      }
      // rainbow
      else if (mode == 2) {
        // given 256 colors, how many colors per pixel or skipped colors between pixels (less than 1.0)
        float pixelsPerColor = pixelCount / 256.0 / rainbowSize;
        int rainbowColor = rainbowPos + (int)(i / pixelsPerColor);
        // show pixelCount number of positions of 255 possible, rotate 255 times then reset
        strip.SetPixelColor(i, Wheel(rainbowColor));
        if (millis() > clockMs + (1000 / rainbowSpeed / rainbowSize)) {
          rainbowPos++;
          if (rainbowPos > 255) {
            rainbowPos = 0;
          }
          clockMs = millis();
        }
      }
      // gradient
      /*##
        use an array of colors with a series of deltas between each color
        to be determined when each color will switch to the next delta
      */
      else if (mode == 3) {
        strip.SetPixelColor(i, RgbwColor(
          ww + (int) (wwDelta * i),
          b + (int) (bDelta * i),
          g + (int) (gDelta * i),
          r + (int) (rDelta * i)));
      }
    }
  }
  strip.Show();  
}

RgbwColor Wheel(uint8_t WheelPos)
{
  WheelPos = 255 - WheelPos;
  float multiplier = rainbowBrightness / 255.0;
  if(WheelPos < 85) 
  {
    
    return RgbwColor(ww, (int)((255 - WheelPos * 3) * multiplier), 0, (int)((WheelPos * 3) * multiplier));
  } else if(WheelPos < 170) 
  {
    WheelPos -= 85;
    return RgbwColor(ww, 0, (int)((WheelPos * 3) * multiplier), (int)((255 - WheelPos * 3) * multiplier));
  } else 
  {
    WheelPos -= 170;
    return RgbwColor(ww, (int)((WheelPos * 3) * multiplier), (int)((255 - WheelPos * 3) * multiplier), 0);
  }
}
