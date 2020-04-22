#include "os.h"
#include "ux.h"
#include "os_io_seproxyhal.h"

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define P1_CONFIRM 0x01
#define P1_NON_CONFIRM 0x00

#define P2_EXTEND 0x01
#define P2_MORE 0x02

#define ROUND_TO_NEXT(x, next) \
    (((x) == 0) ? 0 : ((((x - 1) / (next)) + 1) * (next)))

/* See constant by same name in sdk/src/packet.rs */
#define PACKET_DATA_SIZE (1280 - 40 - 8)

#define MAX_MESSAGE_LENGTH ROUND_TO_NEXT(PACKET_DATA_SIZE, USB_SEGMENT_SIZE)
#define SIGNATURE_LENGTH 64
#define HASH_LENGTH 32
#define PUBKEY_LENGTH HASH_LENGTH
#define BIP32_PATH 5

extern ux_state_t ux;
// display stepped screens
extern unsigned int ux_step;
extern unsigned int ux_step_count;

enum BlindSign {
    BlindSignDisabled = 0,
    BlindSignEnabled = 1,
};

enum PubkeyDisplay {
    PubkeyDisplayLong = 0,
    PubkeyDisplayShort = 1,
};

typedef struct AppSettings {
    uint8_t allow_blind_sign;
    uint8_t pubkey_display;
} AppSettings;

typedef struct internalStorage_t {
    AppSettings settings;
    uint8_t initialized;
} internalStorage_t;

extern const internalStorage_t N_storage_real;
#define N_storage (*(volatile internalStorage_t*) PIC(&N_storage_real))
#endif
