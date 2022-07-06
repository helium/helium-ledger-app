#include "bolos_target.h"

#if defined(TARGET_NANOS) && !defined(HAVE_UX_FLOW)

#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include <cx.h>
#include "helium.h"
#include "helium_ux.h"

static uint16_t adpu_tx = 0;

// Define the comparison screen. This is where the user will compare the
// public key (or address) on their device to the one shown on the computer.
static const bagl_element_t ui_getPublicKey[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, global.title),
	// The visible portion of the public key or address.
	UI_TEXT(0x00, 0, 26, 128, global.partialStr),
};

static void local_load_wallet(page_cmd_t page_cmd);

// Define the button handler for the comparison screen. Again, this is nearly
// identical to the signHash comparison button handler.
static unsigned int ui_getPublicKey_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_LEFT:
	case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
        change_page(
            PREV,
            &local_load_wallet,
            &local_load_wallet
        );

		UX_REDISPLAY();
		break;

	case BUTTON_RIGHT:
	case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
        change_page(
            NEXT,
            &local_load_wallet,
            &local_load_wallet
        );
		UX_REDISPLAY();
		break;

	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // PROCEED
        global.lock = true;
        io_exchange_with_code(SW_OK, adpu_tx);
		// The user has finished comparing, so return to the main screen.
		ui_idle();
		break;
	}
	return 0;
}

static void local_load_wallet(page_cmd_t page_cmd) {
    load_wallet(page_cmd, local_load_wallet, local_load_wallet);
    UX_DISPLAY(ui_getPublicKey, NULL);
}

// handleGetPublicKey is the entry point for the getPublicKey command. It
// reads the command parameters, prepares and displays the approval screen,
// and
void handle_get_public_key(uint8_t p1, uint8_t p2,
                           __attribute__((unused)) uint8_t *dataBuffer,
                           __attribute__((unused)) uint16_t dataLength,
                           __attribute__((unused)) volatile unsigned int *flags,
                           __attribute__((unused)) volatile unsigned int *tx) {
	// Sanity-check the command
	if ((p1 != P1_PUBKEY_DISPLAY_ON) && (p1 != P1_PUBKEY_DISPLAY_OFF)) {
		THROW(SW_INVALID_PARAM);
	}
    global.account_index = p2;

    load_wallet(FIRST,
        local_load_wallet,
        local_load_wallet);

	if (p1 == P1_PUBKEY_DISPLAY_ON) {
        UX_DISPLAY(ui_getPublicKey, NULL);
		// Sets the IO_ASYNC_REPLY, which allows the screen to load
		*flags |= IO_ASYNCH_REPLY;
	} else {
        // Flush the APDU buffer, sending the response.
        io_exchange_with_code(SW_OK, adpu_tx);
    }
}

void load_wallet(page_cmd_t page_cmd,
    void (*prev_menu)(page_cmd_t),
    void (*next_menu)(page_cmd_t)) {
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
        global.title_len = sizeof("Wallet N (1/m)");
        memcpy(global.title, &"Wallet N (1/m)", global.title_len);
        global.title[7] = global.account_index + 48;
    } else if (global.account_index < 100) {
        global.title_len = sizeof("Wallet NN (1/m)");
        memcpy(global.title, &"Wallet NN (1/m)", global.title_len);
        global.title[7] = global.account_index/10 + 48;
        global.title[8] = global.account_index%10 + 48;
    } else {
        global.title_len = sizeof("Wallet NNN (1/m)");
        memcpy(global.title, &"Wallet NNN (1/m)", global.title_len);
        global.title[7] = global.account_index/100 + 48;
        global.title[8] = global.account_index%100/10 + 48;
        global.title[9] = global.account_index%10 + 48;
    }

    change_page(
        page_cmd,
        prev_menu,
        next_menu
    );
}

#endif
