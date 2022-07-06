#include "nanox_wallet.h"

#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include <cx.h>
#include "helium.h"
#include "helium_ux.h"

void init_wallet(void)
{
    uint8_t adpu_tx;
    size_t output_len;
    cx_sha256_t hash;
    unsigned char hash_buffer[32];
    adpu_tx = 2;

    G_io_apdu_buffer[0] = 0; // prepend 0 byte to signify b58 format
#ifdef HELIUM_TESTNET
    G_io_apdu_buffer[1] = NETTYPE_TEST | KEYTYPE_ED25519;
#else
    G_io_apdu_buffer[1] = NETTYPE_MAIN | KEYTYPE_ED25519;
#endif
    get_pubkey_bytes(global.account_index, &G_io_apdu_buffer[adpu_tx]);
    adpu_tx += SIZE_OF_PUB_KEY_BIN;

    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, G_io_apdu_buffer, adpu_tx, hash_buffer, 32);
    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);

    memmove(&G_io_apdu_buffer[adpu_tx], hash_buffer, SIZE_OF_SHA_CHECKSUM);
    adpu_tx += SIZE_OF_SHA_CHECKSUM;

    // for some reason this needs to be done twice to work
    btchip_encode_base58(G_io_apdu_buffer, adpu_tx, global.fullStr, &output_len);
    btchip_encode_base58(G_io_apdu_buffer, adpu_tx, global.fullStr, &output_len);

    global.fullStr[51] = '\0';
    global.fullStr_len = output_len;
    global.displayIndex = 0;

    if (global.account_index < 10) {
        global.title_len = sizeof("Wallet N\0");
        memcpy(global.title, &"Wallet N\0", global.title_len);
        global.title[7] = global.account_index + 48;
    } else if (global.account_index < 100) {
        global.title_len = sizeof("Wallet NN\0");
        memcpy(global.title, &"Wallet NN\0", global.title_len);
        global.title[7] = global.account_index/10 + 48;
        global.title[8] = global.account_index%10 + 48;
    } else {
        global.title_len = sizeof("Wallet NNN\0");
        memcpy(global.title, &"Wallet NNN\0", global.title_len);
        global.title[7] = global.account_index/100 + 48;
        global.title[8] = global.account_index%100/10 + 48;
        global.title[9] = global.account_index%10 + 48;
    }
}