#include "tft_mono.h"

#include <Adafruit_GFX.h>
#include <pgmspace.h>
#include <TFT_ILI9163C.h>

TFT_Mono::TFT_Mono(uint8_t cspin, uint8_t dcpin, uint8_t rstpin) :
  TFT_ILI9163C(cspin, dcpin, rstpin) {
}

void  TFT_Mono::setFont(const GFXfont *f) {
  Adafruit_GFX::setFont(f);
  is_monospaced_ = true;
  if (!f) {
    return;
  }
  x_min_ = 0;
  x_max_ = 0;
  y_min_ = 0;
  y_max_ = 0;
  uint8_t first = pgm_read_byte(&f->first);
  uint8_t last = pgm_read_byte(&f->last);
  uint8_t xa0 = pgm_read_byte(&f->glyph[0].xAdvance);
  for (int i = 0; i <= last - first; ++i) {
    int16_t width_i  = (int8_t)pgm_read_byte(&f->glyph[i].width);
    int16_t height_i  = (int8_t)pgm_read_byte(&f->glyph[i].height);
    x_advance_ = (int8_t)pgm_read_byte(&f->glyph[i].xAdvance);
    int16_t xoffset_i = (int8_t)pgm_read_byte(&f->glyph[i].xOffset);
    int16_t yoffset_i = (int8_t)pgm_read_byte(&f->glyph[i].yOffset);
    if (x_advance_ != xa0) {
      is_monospaced_ = false;
      Serial.print("Not monospaced, difference at ");
      Serial.println(i);
      return;
    }
    x_min_ = min(x_min_, xoffset_i);
    x_max_ = max(x_max_, xoffset_i + width_i);
    y_min_ = min(y_min_, yoffset_i);
    y_max_ = max(y_max_, yoffset_i + height_i);
  }
  y_advance_ = pgm_read_byte(&f->yAdvance);
}

void TFT_Mono::drawChar(int16_t x, int16_t y,
                        unsigned char c,
                        uint16_t color, uint16_t bg,
                        uint8_t size) {
  if (!gfxFont || !is_monospaced_) {
    Adafruit_GFX::drawChar(x, y, c, color, bg, size);
  } else {
    unsigned char cc = c;
    c -= pgm_read_byte(&gfxFont->first);
    GFXglyph* glyph = &(((GFXglyph*)pgm_read_pointer(&gfxFont->glyph))[c]);
    uint8_t* bitmap = (uint8_t*)pgm_read_pointer(&gfxFont->bitmap);
    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width);
    uint8_t h = pgm_read_byte(&glyph->height);
    int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
    int16_t yo = (int8_t)pgm_read_byte(&glyph->yOffset);
    uint16_t xx;
    uint16_t yy;
    uint8_t bits = 0;
    uint8_t bit = 0;
    if (yo > y_min_) {
      fillRect(x + x_min_ * size, y + y_min_ * size,
               (x_max_ - x_min_) * size, yo - y_min_ * size, textbgcolor);
    }
    for (yy = 0; yy < h; yy++) {
      for (int xf = x_min_; xf < xo; ++xf) {
        if (size == 1) {
          drawPixel(x + xf, y + yo + yy, textbgcolor);
        } else {
          fillRect(x + xf * size, y + (yo + yy) * size, size, size, textbgcolor);
        }
      }
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        uint16_t the_color = (bits & 0x80) ? color : textbgcolor;
        if (size == 1) {
          drawPixel(x + xo + xx, y + yo + yy, the_color);
        } else {
          fillRect(x + (xo + xx) * size, y + (yo + yy) * size, size, size, the_color);
        }
        bits <<= 1;
      }
      for (int xf = xo + w; xf < x_max_; ++xf) {
        if (size == 1) {
          drawPixel(x + xf, y + yo + yy, textbgcolor);
        } else {
          fillRect(x + xf * size, y + (yo + yy) * size, size, size, textbgcolor);
        }
      }
    }
    if (yo + h < y_max_) {
      fillRect(x + x_min_ * size, y + (yo + h) * size,
               (x_max_ - x_min_) * size,
               (y_max_ - yo - h) * size, textbgcolor);
    }
  }
}

void TFT_Mono::yAdvance() {
  cursor_x = 0;
  cursor_y += y_advance_ * textsize;
}

void TFT_Mono::clearToEol() {
  fillRect(cursor_x + x_min_ * textsize, cursor_y + y_min_ * textsize,
           (x_max_ - x_min_) * textsize, (y_max_ - y_min_) * textsize, textbgcolor);
}
void TFT_Mono::lineprint(float value, uint8_t integral_digits, uint8_t fractional_digits) {
  // int xc = cursor_x;
  // int yc = cursor_y;
  // setFont(f);
  // cursor_x = xc;
  // cursor_y = yc;

  // Serial.print(String(value) + ", " + String(integral_digits) + ", " + String(fractional_digits));
  bool negative = false;
  if (value < 0) {
    negative = true;
    value = -value;
  }

  // As much as will fit horizontally.
  int space = _width / (x_advance_ * textsize);
  // Serial.println("; space: " + String(space));
  String s;
  int decimal_point = (fractional_digits > 0) ? 1 : 0;
  if (integral_digits + decimal_point + fractional_digits > space) {
    for (int i = 0; i < space; ++i) {
      s += "*";
    }
  } else {
    for (int i = 0; i < integral_digits; ++i) {
      s += '_';
    }
    // Will the integral part fit?
    float tvalue = value;
    for (int i = integral_digits - 1; i >= 0; --i) {
      // Serial.println("i=" + String(i) + "; tvalue=" + String(tvalue));
      char c = (int)tvalue % 10 + '0';
      s.setCharAt(i, c);
      // Serial.println(c);
      tvalue /= 10.0;
    }
    if (tvalue < 1) {
      // It did, strip off leading 0s
      for (int i = 0; i < integral_digits - 1; ++i) {
        if (s[i] == '0') {
          // Serial.println("Stripping digit " + String(i));
          s.setCharAt(i, ' ');
        } else {
          break;
        }
      }
      if (decimal_point) {
        s += '.';
        tvalue = value;
        for (int i = 0; i < fractional_digits; ++i) {
          // Serial.println("i=" + String(i) + "; tvalue=" + String(tvalue));
          tvalue -= (int)tvalue;
          char c = (int)(tvalue * 10) + '0';
          s += c;
          tvalue *= 10;
        }
      }
    } else {
      // no, it didn't, fill it up with *s
      for (int i = 0; i < integral_digits; ++i) {
        s.setCharAt(i, '*');
      }
      if (decimal_point) {
        s += '.';
        for (int i = 0; i < fractional_digits; ++i) {
          s += '*';
        }
      }
    }
  }
  uint16_t fg = textcolor;
  uint16_t bg = textbgcolor;
  if (negative) {
    textcolor = bg;
    textbgcolor = fg;
  }
  print(s);
  if (negative) {
    textcolor = fg;
    textbgcolor = bg;
  }
}

size_t TFT_Mono::write(uint8_t c) {

  if(!gfxFont) { // 'Classic' built-in font
    if(c == '\n') {
      cursor_y += textsize * 8;
      cursor_x = 0;
    } else if(c == '\r') {
      // pass
    } else {
      if (wrap && ((cursor_x + textsize * 6) >= _width)) { // Heading off edge?
        cursor_x = 0;             // Reset x to zero
        cursor_y += textsize * 8; // Advance y one line
      }
      drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
      cursor_x += textsize * 6;
    }
  } else {                      // Custom font
    if(c == '\n') {
      cursor_x  = 0;
      cursor_y += (int16_t)textsize * y_advance_;
    } else if (c == '\r') {
      cursor_x = 0;
    } else {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        uint8_t c2 = c - pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c2]);
        uint8_t w = pgm_read_byte(&glyph->width);
        uint8_t h = pgm_read_byte(&glyph->height);
//        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          if (wrap && ((cursor_x + textsize * (xo + w)) >= _width)) {
            // Drawing character would go off right edge; wrap to new line
            cursor_x = 0;
            cursor_y += (int16_t)textsize * y_advance_;
          }
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
//        }
        cursor_x += (int16_t)textsize * x_advance_;
      }
    }
  }
  return 1;
}
