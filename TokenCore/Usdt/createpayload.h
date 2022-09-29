#ifndef CREATEPAYLOAD_H
#define CREATEPAYLOAD_H

#include "TokenCommon.h"

string CreatePayload_SimpleSend(uint32_t propertyId, uint64_t amount);
string CreatePayload_SendAll(uint8_t ecosystem);

#endif // CREATEPAYLOAD_H
