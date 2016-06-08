#ifndef _OTA_CB_H_
#define _OTA_CB_H_

#include "options.h"

#if USE_WIFI == 1
#include <ArduinoOTA.h>

void ota_on_start();
void ota_on_progress(unsigned int total, unsigned int size);
void ota_on_error(ota_error_t e);
void ota_on_end();
#endif
#endif
