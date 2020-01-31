#include "os.h"
#include "cx.h"
#include "globals.h"

#ifndef _GET_PUBKEY_H_
#define _GET_PUBKEY_H_

void handleGetPubkey(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx);

#endif
