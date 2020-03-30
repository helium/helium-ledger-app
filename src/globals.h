#include "os.h"
#include "ux.h"
#include "os_io_seproxyhal.h"

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define P1_CONFIRM 0x01
#define P1_NON_CONFIRM 0x00

#define P2_EXTEND 0x01
#define P2_MORE 0x02

#define MAX_MESSAGE_LENGTH 1000
#define SIGNATURE_LENGTH 64
#define HASH_LENGTH 32
#define PUBKEY_LENGTH HASH_LENGTH
#define BIP32_PATH 5

extern ux_state_t ux;
// display stepped screens
extern unsigned int ux_step;
extern unsigned int ux_step_count;

typedef struct internalStorage_t {
    unsigned char dummy_setting_1;
    unsigned char dummy_setting_2;
    uint8_t initialized;
} internalStorage_t;

extern const internalStorage_t N_storage_real;
#define N_storage (*(volatile internalStorage_t*) PIC(&N_storage_real))
#endif
