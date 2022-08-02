#include "os.h"
#include "cx.h"
#include "globals.h"

#ifndef _SIGN_MESSAGE_H_
#define _SIGN_MESSAGE_H_

extern uint8_t G_numDerivationPaths;

void handle_sign_message_receive_apdus(uint8_t p1,
                                       uint8_t p2,
                                       const uint8_t *dataBuffer,
                                       uint16_t dataLength);

void handle_sign_message_parse_message(volatile unsigned int *tx);

void handle_sign_message_UI(volatile unsigned int *flags);

#endif
