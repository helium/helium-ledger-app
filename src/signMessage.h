#include "os.h"
#include "cx.h"
#include "globals.h"

#ifndef _SIGN_MESSAGE_H_
#define _SIGN_MESSAGE_H_

extern uint8_t G_numDerivationPaths;

void handleSignMessage(
    uint8_t p1,
    uint8_t p2,
    uint8_t *dataBuffer,
    uint16_t dataLength,
    volatile unsigned int *flags,
    volatile unsigned int *tx
);

#endif
