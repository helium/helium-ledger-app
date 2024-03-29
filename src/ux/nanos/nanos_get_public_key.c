#include "bolos_target.h"

#if defined(TARGET_NANOS) && !defined(HAVE_UX_FLOW)

#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include <cx.h>
#include "helium.h"
#include "helium_ux.h"

// Get a pointer to getPublicKey's state variables.
#define CTX global.getPublicKeyContext

// Define the comparison screen. This is where the user will compare the
// public key (or address) on their device to the one shown on the computer.
static const bagl_element_t ui_getPublicKey[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, "Confirm Address"),
	// The visible portion of the public key or address.
	UI_TEXT(0x00, 0, 26, 128, CTX.partialStr),
};

// Define the preprocessor for the comparison screen. As in signHash, this
// preprocessor selectively hides the left/right arrows. The only difference
// is that, since public keys and addresses have different lengths, checking
// for the end of the string is slightly more complicated.
static const bagl_element_t* ui_prepro_getPublicKey(const bagl_element_t *element) {
	int fullSize =  CTX.fullStr_len;
	if ((element->component.userid == 1 && CTX.displayIndex == 0) ||
	    (element->component.userid == 2 && CTX.displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

// Define the button handler for the comparison screen. Again, this is nearly
// identical to the signHash comparison button handler.
static unsigned int ui_getPublicKey_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	int fullSize = CTX.fullStr_len;
	switch (button_mask) {
	case BUTTON_LEFT:
	case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
		if (CTX.displayIndex > 0) {
			CTX.displayIndex--;
		}
		memmove(CTX.partialStr, CTX.fullStr+CTX.displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_RIGHT:
	case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
		if (CTX.displayIndex < fullSize-12) {
			CTX.displayIndex++;
		}
		memmove(CTX.partialStr, CTX.fullStr+CTX.displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // PROCEED
		// The user has finished comparing, so return to the main screen.
		ui_idle();
		break;
	}
	return 0;
}

// handleGetPublicKey is the entry point for the getPublicKey command. It
// reads the command parameters, prepares and displays the approval screen,
// and 
void handle_get_public_key(uint8_t p1, uint8_t p2,
                           __attribute__((unused)) uint8_t *dataBuffer,
                           __attribute__((unused)) uint16_t dataLength,
                           __attribute__((unused)) volatile unsigned int *flags,
                           __attribute__((unused)) volatile unsigned int *tx) {
	size_t output_len;
	// Sanity-check the command parameters.
	if ((p1 != P1_PUBKEY_DISPLAY_ON) && (p1 != P1_PUBKEY_DISPLAY_OFF)) {
		THROW(SW_INVALID_PARAM);
	}
	uint16_t adpu_tx = 2;

	G_io_apdu_buffer[0] = 0; // prepend 0 byte to signify b58 format
#ifdef HELIUM_TESTNET
	G_io_apdu_buffer[1] = NETTYPE_TEST | KEYTYPE_ED25519;
#else
	G_io_apdu_buffer[1] = NETTYPE_MAIN | KEYTYPE_ED25519;
#endif
	uint8_t account = p2;
	get_pubkey_bytes(account, &G_io_apdu_buffer[adpu_tx]);
	adpu_tx += SIZE_OF_PUB_KEY_BIN;

	cx_sha256_t hash;
	unsigned char hash_buffer[32];

	cx_sha256_init(&hash);
	cx_hash(&hash.header, CX_LAST, G_io_apdu_buffer, adpu_tx, hash_buffer, 32);
	cx_sha256_init(&hash);
	cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);

	memmove(&G_io_apdu_buffer[adpu_tx], hash_buffer, SIZE_OF_SHA_CHECKSUM);
	adpu_tx += SIZE_OF_SHA_CHECKSUM;


	if (p1 == P1_PUBKEY_DISPLAY_ON) {
		// for some reason this needs to run twice to get the display to work
		// otherwise, first time running this command, key gets displayed blank
		for(uint8_t i=0; i<2; i++){
			btchip_encode_base58(G_io_apdu_buffer, adpu_tx, CTX.fullStr, &output_len);
			CTX.fullStr[51] = '\0';
			CTX.fullStr_len = output_len;
			memmove(CTX.partialStr, CTX.fullStr, 12);
			CTX.partialStr[12] = '\0';
			CTX.displayIndex = 0;

			// Display the comparison screen.
			UX_DISPLAY(ui_getPublicKey, ui_prepro_getPublicKey);
		}

		// Sets the IO_ASYNC_REPLY, which allows the screen to load
		*flags |= IO_ASYNCH_REPLY;
	}

	// Flush the APDU buffer, sending the response.
	io_exchange_with_code(SW_OK, adpu_tx);
}

#endif
