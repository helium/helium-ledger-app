#include "bolos_target.h"

#if defined(TARGET_NANOS) && !defined(HAVE_UX_FLOW)

#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include <cx.h>
#include "helium.h"
#include "helium_ux.h"
#include "save_context.h"
#include "nanos_error.h"

#define CTX cmd.unstakeValidatorContext

static void load_amount(page_cmd_t page_cmd);
static void load_recipient(page_cmd_t page_cmd);
static void load_release_height(page_cmd_t page_cmd);
static void load_fee(page_cmd_t page_cmd);
static void load_signature(page_cmd_t page_cmd);
static void load_deny(page_cmd_t page_cmd);
static void local_load_wallet(page_cmd_t page_cmd);

static const bagl_element_t ui_signTxn_approve[] = {
	UI_BACKGROUND(),
    UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_ICON_CENTER(0x00, BAGL_GLYPH_ICON_CHECK),
	UI_TEXT(0x00, 0, 12, 128, "Approve?"),
};


static const bagl_element_t ui_signTxn_deny[] = {
	UI_BACKGROUND(),
    UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_ICON_CENTER(0x00, BAGL_GLYPH_ICON_CROSS),
	UI_TEXT(0x00, 0, 12, 128, "Deny transaction?"),
};

static unsigned int ui_signTxn_deny_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_LEFT | BUTTON_EVT_RELEASED:
        if(!global.lock) {
            load_signature(LAST);
            UX_REDISPLAY();
        }
		break;
	case BUTTON_LEFT | BUTTON_EVT_FAST:
        break;
	case BUTTON_RIGHT | BUTTON_EVT_RELEASED:
        if(!global.lock) {
            local_load_wallet(FIRST);
            UX_REDISPLAY();
        }
		break;
	case BUTTON_RIGHT | BUTTON_EVT_FAST: // SEEK RIGHT
        break;
	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // keeps the screen from flipping around after being denied
        global.lock = true;
        // make sure there's no data in the buffer
		memset(G_io_apdu_buffer, 0, IO_APDU_BUFFER_SIZE);
		// send a single 0 byte to differentiate from app not running
		io_exchange_with_code(SW_OK, 1);
		ui_idle();
		break;
	}
	return 0;
}

static unsigned int ui_signTxn_approve_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	int adpu_tx;
	switch (button_mask) {
	case BUTTON_LEFT | BUTTON_EVT_RELEASED:
        if(!global.lock) {
            load_fee(LAST);
        }
		break;
	case BUTTON_RIGHT | BUTTON_EVT_RELEASED:
        if(!global.lock) {
            load_deny(LAST);
        }
		break;
	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        global.lock = true;
        adpu_tx = create_helium_unstake_txn(global.account_index);
		io_exchange_with_code(SW_OK, adpu_tx);
		ui_idle();
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayFee[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, global.title),
	// The visible portion of fee
	UI_TEXT(0x00, 0, 26, 128, global.partialStr),
};

static unsigned int ui_displayFee_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_LEFT | BUTTON_EVT_RELEASED: // SEEK LEFT
        change_page(
            PREV,
            &load_release_height,
            &load_signature
        );
		UX_REDISPLAY();
		break;
    case BUTTON_RIGHT | BUTTON_EVT_RELEASED: // SEEK RIGHT
        change_page(
            NEXT,
            &load_release_height,
            &load_signature
        );
        UX_REDISPLAY();
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayReleaseHeight[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, global.title),
	// The visible portion of fee
	UI_TEXT(0x00, 0, 26, 128, global.partialStr),
};

static unsigned int ui_displayReleaseHeight_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_LEFT | BUTTON_EVT_RELEASED: // SEEK LEFT
        change_page(
            PREV,
            &load_recipient,
            &load_fee
        );
        UX_REDISPLAY();
        break;
	case BUTTON_RIGHT | BUTTON_EVT_RELEASED: // SEEK RIGHT
        change_page(
            NEXT,
            &load_recipient,
            &load_fee
        );
		UX_REDISPLAY();
		break;
    }
	return 0;
}

static const bagl_element_t ui_displayRecipient[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, global.title),
	// The visible portion of the recipient
	UI_TEXT(0x00, 0, 26, 128, global.partialStr),
};

static unsigned int ui_displayRecipient_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_LEFT | BUTTON_EVT_RELEASED: // SEEK LEFT
        change_page(
            PREV,
            &load_amount,
            &load_release_height
        );
		UX_REDISPLAY();
		break;
	case BUTTON_RIGHT | BUTTON_EVT_RELEASED: // SEEK RIGHT
        change_page(
            NEXT,
            &load_amount,
            &load_release_height
        );
		UX_REDISPLAY();
		break;
    }
	return 0;
}

static const bagl_element_t ui_displayAmount[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, global.title),
	// The visible portion of the amount
	UI_TEXT(0x00, 0, 26, 128, global.partialStr),
};

static unsigned int ui_displayAmount_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
    switch (button_mask) {
	case BUTTON_LEFT | BUTTON_EVT_RELEASED: // SEEK LEFT
        change_page(
            PREV,
            &local_load_wallet,
            &load_recipient
        );
        UX_REDISPLAY();
		break;
	case BUTTON_RIGHT | BUTTON_EVT_RELEASED: // SEEK RIGHT
        change_page(
            NEXT,
            &local_load_wallet,
            &load_recipient
        );
        UX_REDISPLAY();
		break;
    }
	return 0;
}

static const bagl_element_t ui_getPublicKey[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, global.title),
	// The visible portion of the public key or address.
	UI_TEXT(0x00, 0, 26, 128, global.partialStr),
};

// Define the button handler for the comparison screen. Again, this is nearly
// identical to the signHash comparison button handler.
static unsigned int ui_getPublicKey_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_LEFT | BUTTON_EVT_RELEASED: // SEEK LEFT
        if (!global.lock) {
            change_page(
                PREV,
                &load_deny,
                &load_amount
            );
        }
		UX_REDISPLAY();
		break;
	case BUTTON_RIGHT | BUTTON_EVT_RELEASED: // SEEK RIGHT
        if (!global.lock) {
            change_page(
                NEXT,
                &load_deny,
                &load_amount
            );
        }
		UX_REDISPLAY();
		break;
    }
	return 0;
}

static void local_load_wallet(page_cmd_t page_cmd) {
    load_wallet(page_cmd, load_deny, load_amount);
    UX_DISPLAY(ui_getPublicKey, NULL);
}

void handle_unstake_validator_txn(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength,
                             volatile unsigned int *flags, __attribute__((unused)) volatile unsigned int *tx) {

    if(save_unstake_validator_context(p1, p2, dataBuffer, dataLength, &CTX)) {
        local_load_wallet(FIRST);
    } else {
        display_error();
    }
	*flags |= IO_ASYNCH_REPLY;
}

static void load_fee(page_cmd_t page_cmd) {
    uint8_t len = bin2dec(global.fullStr, CTX.fee);
    global.title_len = sizeof("DC Fee (1/n)");
    memcpy(global.title, &"DC Fee (1/n)", global.title_len);
    global.fullStr_len = len;
    global.fullStr[len] = '\0';
    global.displayIndex = 0;
    change_page(
        page_cmd,
        &load_release_height,
        &load_signature
    );
    UX_DISPLAY(ui_displayFee, NULL);
}

static void load_recipient(page_cmd_t page_cmd) {
	cx_sha256_t hash;
	unsigned char hash_buffer[32];
	// use the G_io_apdu buffer as a scratchpad to minimize stack usage
	uint8_t * address_with_check = G_io_apdu_buffer;
    global.title_len = sizeof("Validator Addr (1/n)");
    memcpy(global.title, &"Validator Addr (1/n)", global.title_len);
    for(uint8_t i=0; i<2; i++){
        memmove(address_with_check, CTX.address, 34);
        cx_sha256_init(&hash);
        cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
        cx_sha256_init(&hash);
        cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
        memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
        size_t output_len;
        btchip_encode_base58(address_with_check, 38, global.fullStr, &output_len);
        global.fullStr[output_len] = '\0';
        global.fullStr_len = output_len;
        change_page(
            page_cmd,
            &load_amount,
            &load_release_height
        );
    }
    UX_DISPLAY(ui_displayRecipient, NULL);
}

static void load_release_height(page_cmd_t page_cmd){
    uint8_t len;
    global.title_len = sizeof("Release Height (1/n)");
    memcpy(global.title, &"Release Height (1/n)", global.title_len);
    len = bin2dec(global.fullStr, CTX.stake_release_height);
    global.fullStr_len = len;
    global.fullStr[len] = '\0';

    change_page(
        page_cmd,
        &load_recipient,
        &load_fee
    );
    UX_DISPLAY(ui_displayReleaseHeight, NULL);
}

static void load_amount(page_cmd_t page_cmd){
    #ifdef HELIUM_TESTNET
    global.title_len = sizeof("Unstake TNT (1/n)");
    memcpy(global.title, &"Unstake TNT (1/n)", global.title_len);
    #else
    global.title_len = sizeof("Unstake HNT (1/n)");
    memcpy(global.title, &"Unstake HNT (1/n)", global.title_len);
    #endif
    // display amount on screen
    uint8_t len = pretty_print_hnt(global.fullStr, CTX.stake_amount);
    global.fullStr_len = len;
    change_page(
        page_cmd,
        &load_signature,
        &load_recipient
    );
    UX_DISPLAY(ui_displayAmount, NULL);
}

static void load_signature(__attribute__((unused)) page_cmd_t page_cmd) {
    UX_DISPLAY(ui_signTxn_approve, NULL);
}

static void load_deny(__attribute__((unused)) page_cmd_t page_cmd) {
    UX_DISPLAY(ui_signTxn_deny, NULL);
}

#endif
