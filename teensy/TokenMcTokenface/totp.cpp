// Author: John Ioannidis <ji@tla.org>
// License: Beer

#include "base32.h"

#include <avr/pgmspace.h>
#include <TOTP.h>
#include <Time.h>

#include <ctype.h>
#include <memory>

static char totp_buffer[7];
const char* totp(const String& secret, time_t t) {
  size_t slen = secret.length();
  size_t new_len = 0;
  std::unique_ptr<char[]> upper_secret(new char[slen + 1]);
  for (int i = 0; i < slen; ++i) {
    char c = toupper(secret.charAt(i));
    if ((c >= 'A') && (c <= 'Z') || ((c >= '2') && (c <= '7'))) {
      upper_secret[new_len] = c;
      ++new_len;
    }
  }
  upper_secret[new_len] = '\0';
  size_t decoded_length = new_len * 5 / 8 + 5;
  std::unique_ptr<char[]> decoded_secret(new char [decoded_length]);
  base32_decode(upper_secret.get(), new_len, decoded_secret.get(), &decoded_length);
  String otp = TOTP(reinterpret_cast<uint8_t*>(decoded_secret.get()), decoded_length).getCode(t);
  for (int i = 0; i < 6; ++i) {
    totp_buffer[i] = otp.charAt(i);
  }
  totp_buffer[6] = '\0';
  return totp_buffer;
}
