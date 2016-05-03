//#include <WString.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "base2N.h"
#define PROGMEM
static char b16_symbolset[] PROGMEM = "0123456789ABCDEF";
static char b32_symbolset[] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

int  b32_encode(char* src, char* dst, size_t len) {
  int leftovers = len % 5;
  int steps = len / 5;
  int tlen = 0;
  while (steps--) {
    //    76543
    // ...00000
    dst[0] = b32_symbolset[src[0] >> 3];
    //    210           76
    // ...000.. | ......11
    dst[1] = b32_symbolset[((src[0] & 0x07) << 2) | (src[1] >> 6)];
    //    54321
    // ...11111
    dst[2] = b32_symbolset[(src[1] & 0x3e) >> 1];
    //    0           7654
    // ...1.... | ....2222
    dst[3] = b32_symbolset[((src[1] & 0x1) << 4) | (src[2] >> 4)];
    //    3210           7
    // ...2222. | .......3
    dst[4] = b32_symbolset[((src[2] & 0x0f) << 1) | (src[3] >> 7)];
    //    65432
    // ...33333
    dst[5] = b32_symbolset[((src[3] & 0x7c) >> 2)];
    //    10           765
    // ...33... | .....444
    dst[6] = b32_symbolset[((src[3] & 0x03) << 3) | (src[4] >> 5)];
    //    43210
    // ...44444
    dst[7] = b32_symbolset[src[4] & 0x1f];
    src += 5;
    dst += 8;
    tlen += 8;
  }
  switch (leftovers) {
  case 0:
    tlen -= 8;
    break;
  case 1:
    dst[0] = b32_symbolset[src[0] >> 3];
    dst[1] = b32_symbolset[(src[0] & 0x07) << 2];
    dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = '=';
    break;
  case 2:
    dst[0] = b32_symbolset[src[0] >> 3];
    dst[1] = b32_symbolset[((src[0] & 0x07) << 2) | (src[1] >> 6)];
    dst[2] = b32_symbolset[(src[1] & 0x3f) >> 1];
    dst[3] = b32_symbolset[(src[1] & 0x1) << 4];
    dst[4] = dst[5] = dst[6] = dst[7] = '=';
    break;
  case 3:
    dst[0] = b32_symbolset[src[0] >> 3];
    dst[1] = b32_symbolset[((src[0] & 0x07) << 2) | (src[1] >> 6)];
    dst[2] = b32_symbolset[(src[1] & 0x3f) >> 1];
    dst[3] = b32_symbolset[((src[1] & 0x1) << 4) | (src[2] >> 4)];
    dst[4] = b32_symbolset[(src[2] & 0x0f) << 1];
    dst[5] = dst[6] = dst[7] = '=';
  case 4:
    dst[0] = b32_symbolset[src[0] >> 3];
    dst[1] = b32_symbolset[((src[0] & 0x07) << 2) | (src[1] >> 6)];
    dst[2] = b32_symbolset[(src[1] & 0x3f) >> 1];
    dst[3] = b32_symbolset[((src[1] & 0x1) << 4) | (src[2] >> 4)];
    dst[4] = b32_symbolset[((src[2] & 0x0f) << 1) | (src[3] >> 7)];
    dst[5] = b32_symbolset[((src[3] & 0x7f) >> 2)];
    dst[6] = b32_symbolset[(src[3] & 0x03) << 3];
    dst[7] = '=';
    break;
  }
  tlen += 8;
  return tlen;
}

int main(int argc, char* argv[]) {
  char buf[80];
  int n = b32_encode(argv[1], buf, strlen(argv[1]));
  buf[n] = 0;
  printf("%d: %s\n", n, buf);
}
