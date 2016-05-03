#ifndef _BASE2N_H_
#define _BASE2N_H_
bool b16encode(const uint8_t* src, uint8_t* dst, size_t len);
bool b16decode(const uint8_t* src, uint8_t* dst, size_t len);
bool b32encode(const uint8_t* src, uint8_t* dst, size_t len);
bool b32decode(const uint8_t* src, uint8_t* dst, size_t len);
bool b64encode(const uint8_t* src, uint8_t* dst, size_t len);
bool b64decode(const uint8_t* src, uint8_t* dst, size_t len);
#endif
