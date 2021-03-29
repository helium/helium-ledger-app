#include "bolos_target.h"

#ifdef HAVE_UX_FLOW

#include <stdint.h>
#include <os.h>
#include "helium.h"
#include "helium_ux.h"

// Get a pointer to getPublicKey's state variables.
static getPublicKeyContext_t *ctx = &global.getPublicKeyContext;

UX_FLOW_DEF_VALID(
    ux_display_public_flow_1_step, 
    bnnn_paging,
    ui_idle(),
    {
      .title = "Confirm Address",
      .text = (char *)global.getPublicKeyContext.fullStr
    });


UX_DEF(ux_display_public_flow,
  &ux_display_public_flow_1_step
);

void ui_getPublicKey(void)
{
  if(G_ux.stack_count == 0) {
    ux_stack_push();
  }
  ux_flow_init(0, ux_display_public_flow, NULL);
}

// handleGetPublicKey is the entry point for the getPublicKey command. It
// reads the command parameters, prepares and displays the approval screen,
// and 
void handle_get_public_key(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
	size_t output_len;
	// Sanity-check the command parameters.
	if ((p1 != P1_PUBKEY_DISPLAY_ON) && (p1 != P1_PUBKEY_DISPLAY_OFF)) {
		THROW(SW_INVALID_PARAM);
	}
	uint16_t adpu_tx = 2;

	G_io_apdu_buffer[0] = 0; // prepend 0 byte to signify b58 format
	G_io_apdu_buffer[1] = 1; // set wallet type prefix to KEYTYPE_ED25519
	get_pubkey_bytes(&G_io_apdu_buffer[adpu_tx]);
	adpu_tx += SIZE_OF_PUB_KEY_BIN;

	cx_sha256_t hash;
	unsigned char hash_buffer[32];

	cx_sha256_init(&hash);
	cx_hash(&hash.header, CX_LAST, G_io_apdu_buffer, adpu_tx, hash_buffer, 32);
	cx_sha256_init(&hash);
	cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);

	os_memmove(&G_io_apdu_buffer[adpu_tx], hash_buffer, SIZE_OF_SHA_CHECKSUM);
	adpu_tx += SIZE_OF_SHA_CHECKSUM;

	// Encoding key as a base58 string
	btchip_encode_base58(G_io_apdu_buffer, adpu_tx, ctx->fullStr, &output_len);
	ctx->fullStr[51] = '\0';

	// Running the flow showing the pubkey in the screen
	ui_getPublicKey();

	
	*flags |= IO_ASYNCH_REPLY;

	// Flush the APDU buffer, sending the response.
	io_exchange_with_code(SW_OK, adpu_tx);
}

#endif
