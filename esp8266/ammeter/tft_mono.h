#ifndef _TFT_MONO_H
#define _TFT_MONO_H

#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

class TFT_Mono : public TFT_ILI9163C {
public:
  TFT_Mono(uint8_t cspin, uint8_t dcpin, uint8_t rstpin=255);
  virtual void setFont(const GFXfont *f = NULL);
  virtual void  drawChar(int16_t x, int16_t y,
                         unsigned char c,
                         uint16_t color, uint16_t bg,
                         uint8_t size);
  virtual void yAdvance();
  virtual void clearToEol();
  virtual size_t write(uint8_t);
  virtual void lineprint(float value, uint8_t integral_digits, uint8_t fractional_digits);

protected:
  bool is_monospaced_;
  int16_t x_advance_;
  int16_t y_advance_;
  int16_t x_min_;
  int16_t x_max_;
  int16_t y_min_;
  int16_t y_max_;
};

#endif
