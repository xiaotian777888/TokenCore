#include "createpayload.h"

/**
 * Pushes bytes to the end of a vector.
 */
#define PUSH_BACK_BYTES(vector, value)\
    vector.insert(vector.end(), reinterpret_cast<unsigned char *>(&(value)),\
    reinterpret_cast<unsigned char *>(&(value)) + sizeof((value)));

/**
 * Checks, wether the system uses big or little endian.
 */
static bool isBigEndian()
{
  union
  {
    uint32_t i;
    char c[4];
  } bint = {0x01020304};

  return 1 == bint.c[0];
}

/**
 * Swaps byte order of 16 bit wide numbers on little-endian sytems.
 */
void SwapByteOrder16(uint16_t& us)
{
  if (isBigEndian()) return;

    us = (us >> 8) |
         (us << 8);
}

/**
 * Swaps byte order of 32 bit wide numbers on little-endian sytems.
 */
void SwapByteOrder32(uint32_t& ui)
{
  if (isBigEndian()) return;

    ui = (ui >> 24) |
         ((ui << 8) & 0x00FF0000) |
         ((ui >> 8) & 0x0000FF00) |
         (ui << 24);
}

/**
 * Swaps byte order of 64 bit wide numbers on little-endian sytems.
 */
void SwapByteOrder64(uint64_t& ull)
{
  if (isBigEndian()) return;

    ull = (ull >> 56) |
          ((ull << 40) & 0x00FF000000000000) |
          ((ull << 24) & 0x0000FF0000000000) |
          ((ull << 8)  & 0x000000FF00000000) |
          ((ull >> 8)  & 0x00000000FF000000) |
          ((ull >> 24) & 0x0000000000FF0000) |
          ((ull >> 40) & 0x000000000000FF00) |
          (ull << 56);
}

string CreatePayload_SimpleSend(uint32_t propertyId, uint64_t amount)
{
    Binary payload;
    uint16_t messageType = 0;
    uint16_t messageVer = 0;
    SwapByteOrder16(messageType);
    SwapByteOrder16(messageVer);
    SwapByteOrder32(propertyId);
    SwapByteOrder64(amount);

    PUSH_BACK_BYTES(payload, messageVer);
    PUSH_BACK_BYTES(payload, messageType);
    PUSH_BACK_BYTES(payload, propertyId);
    PUSH_BACK_BYTES(payload, amount);

    return Binary::encode(payload);
}

string CreatePayload_SendAll(uint8_t ecosystem)
{
	Binary payload;
    uint16_t messageVer = 0;
    uint16_t messageType = 4;
    SwapByteOrder16(messageVer);
    SwapByteOrder16(messageType);

    PUSH_BACK_BYTES(payload, messageVer);
    PUSH_BACK_BYTES(payload, messageType);
    PUSH_BACK_BYTES(payload, ecosystem);

    return Binary::encode(payload);
}
