#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include "txns/helium.h"
#include "ux/helium_ux.h"

// handleGetVersion is the entry point for the getVersion command. It
// unconditionally sends the app version.
void handle_get_version(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
	UNUSED(p1); UNUSED(p2); UNUSED(dataBuffer); UNUSED(dataLength); UNUSED(flags); UNUSED(tx);
    G_io_apdu_buffer[0] = APPVERSION[0] - '0';
	G_io_apdu_buffer[1] = APPVERSION[2] - '0';
	G_io_apdu_buffer[2] = APPVERSION[4] - '0';
#ifdef HELIUM_TESTNET
    G_io_apdu_buffer[3] = 'T';
#else
    G_io_apdu_buffer[3] = 'M';
#endif
    io_exchange_with_code(SW_OK, 4);
}
