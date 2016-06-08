// Excerpts from ~/Arduino/libraries/droidfonts/droid.cpp

#include <Adafruit_GFX.h>

//#include <droid.h>

#include "DroidSansMono-6-7b.h"
//#include <fonts/DroidSansMono-10-7b.h>
#include "DroidSansMono-16-7b.h"

const GFXfont* DroidSansMono_6 = &DroidSansMono6pt7b;
//const GFXfont* DroidSansMono_10 = &DroidSansMono10pt7b;
const GFXfont* DroidSansMono_16 = &DroidSansMono16pt7b;

const GFXfont* u_font = DroidSansMono_6;
//const GFXfont* u_font = DroidSansMono_10;
const GFXfont* d_font = DroidSansMono_16;
