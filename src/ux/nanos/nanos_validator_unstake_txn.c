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

#define CTX global.unstakeValidatorContext

static const bagl_element_t ui_signTxn_approve[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x00, BAGL_GLYPH_ICON_CROSS),
	UI_ICON_RIGHT(0x00, BAGL_GLYPH_ICON_CHECK),

	UI_TEXT(0x00, 0, 18, 128, "Sign transaction?"),
};

static const bagl_element_t* ui_prepro_signTxn_approve(const bagl_element_t *element) {
	int fullSize = CTX.fullStr_len;
	if ((element->component.userid == 1 && CTX.displayIndex == 0) ||
	    (element->component.userid == 2 && CTX.displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_signTxn_approve_button(unsigned int button_mask,  __attribute__((unused)) unsigned int button_mask_counter) {
	int adpu_tx;
	switch (button_mask) {
	case BUTTON_LEFT:
	case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
		// make sure there's no data in the office
		memset(G_io_apdu_buffer, 0, IO_APDU_BUFFER_SIZE);
		// send a single 0 byte to differentiate from app not running
		io_exchange_with_code(SW_OK, 1);
		ui_idle();
		break;

	case BUTTON_RIGHT:
	case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
		adpu_tx = create_helium_unstake_txn(CTX.account_index);
		io_exchange_with_code(SW_OK, adpu_tx);
		ui_idle();
		break;

	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayFee[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, "Data Credit Fee"),
	// The visible portion of fee
	UI_TEXT(0x00, 0, 26, 128, CTX.partialStr),
};

static const bagl_element_t* ui_prepro_displayFee(const bagl_element_t *element) {
	int fullSize = CTX.fullStr_len;
	if ((element->component.userid == 1 && CTX.displayIndex == 0) ||
	    (element->component.userid == 2 && CTX.displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_displayFee_button(unsigned int button_mask,  __attribute__((unused)) unsigned int button_mask_counter) {
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
		// display approval screen
		UX_DISPLAY(ui_signTxn_approve, ui_prepro_signTxn_approve);
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayUnstakeAddress[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, "Unstake Address"),
	// The visible portion of the recipient
	UI_TEXT(0x00, 0, 26, 128, CTX.partialStr),
};

static const bagl_element_t* ui_prepro_displayUnstakeAddress(const bagl_element_t *element) {
	int fullSize = CTX.fullStr_len;
	if ((element->component.userid == 1 && CTX.displayIndex == 0) ||
	    (element->component.userid == 2 && CTX.displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_displayUnstakeAddress_button(unsigned int button_mask,  __attribute__((unused)) unsigned int button_mask_counter) {
	int fullSize = CTX.fullStr_len;
	uint8_t len;
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

		// display data credit transaction fee
		len = bin2dec(CTX.fullStr, CTX.fee);
		CTX.fullStr_len = len;
		CTX.fullStr[len] = '\0';
		
		uint8_t partlen = 12;
		if(len < 12){
			partlen = len;
		}
		memmove(CTX.partialStr, CTX.fullStr, partlen);
		CTX.partialStr[partlen] = '\0';
		CTX.displayIndex = 0;

		UX_DISPLAY(ui_displayFee, ui_prepro_displayFee);
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayReleaseHeight[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, "Stake Release Height"),
	// The visible portion of the recipient
	UI_TEXT(0x00, 0, 26, 128, CTX.partialStr),
};

static const bagl_element_t* ui_prepro_displayReleaseHeight(const bagl_element_t *element) {
	int fullSize = CTX.fullStr_len;
	if ((element->component.userid == 1 && CTX.displayIndex == 0) ||
	    (element->component.userid == 2 && CTX.displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_displayReleaseHeight_button(unsigned int button_mask,  __attribute__((unused)) unsigned int button_mask_counter) {
	int fullSize = CTX.fullStr_len;
	cx_sha256_t hash;
	unsigned char hash_buffer[32];
		// use the G_io_apdu buffer as a scratchpad to minimize stack usage
	uint8_t * address_with_check = G_io_apdu_buffer;

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

	    for(uint8_t i=0; i<2; i++){
			// display recipient address on screen
			memmove(address_with_check, CTX.address, 34);

			cx_sha256_init(&hash);
			cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
			cx_sha256_init(&hash);
			cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
			memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
			size_t output_len;
			btchip_encode_base58(address_with_check, 38, CTX.fullStr, &output_len);
			CTX.fullStr[output_len] = '\0';
			CTX.fullStr_len = output_len;
			memmove(CTX.partialStr, CTX.fullStr, 12);
			CTX.partialStr[12] = '\0';
			CTX.displayIndex = 0;

			UX_DISPLAY(ui_displayUnstakeAddress, ui_prepro_displayUnstakeAddress);
		}
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayAmount[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
#ifdef HELIUM_TESTNET
	UI_TEXT(0x00, 0, 12, 128, "Unstake TNT"),
#else
	UI_TEXT(0x00, 0, 12, 128, "Unstake HNT"),
#endif
	// The visible portion of the amount
	UI_TEXT(0x00, 0, 26, 128, CTX.partialStr),
};

static const bagl_element_t* ui_prepro_displayAmount(const bagl_element_t *element) {
	int fullSize = CTX.fullStr_len;
	if ((element->component.userid == 1 && CTX.displayIndex == 0) ||
	    (element->component.userid == 2 && CTX.displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_displayAmount_button(unsigned int button_mask,  __attribute__((unused)) unsigned int button_mask_counter) {
	int fullSize = CTX.fullStr_len;
	uint8_t len;

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

        // display release height of funds
		len = bin2dec(CTX.fullStr, CTX.stake_release_height);
		CTX.fullStr_len = len;
		CTX.fullStr[len] = '\0';

		uint8_t partlen = 12;
		if(len < 12){
			partlen = len;
		}
		memmove(CTX.partialStr, CTX.fullStr, partlen);
		CTX.partialStr[partlen] = '\0';
		CTX.displayIndex = 0;

		UX_DISPLAY(ui_displayReleaseHeight, ui_prepro_displayReleaseHeight);

		break;
	}
	return 0;
}

void handle_unstake_validator_txn(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, __attribute__((unused)) volatile unsigned int *tx) {
    if(save_unstake_validator_context(p1,p2,dataBuffer, dataLength, &CTX)) {
        // display amount on screen
        uint8_t len = pretty_print_hnt(CTX.fullStr, CTX.stake_amount);
        uint8_t i = 0;
        while(CTX.fullStr[i] != '\0' && i<12){
            CTX.partialStr[i] = CTX.fullStr[i];
            i++;
        }
        CTX.partialStr[i] = '\0';
        CTX.fullStr_len = len;

        CTX.displayIndex = 0;

        UX_DISPLAY(ui_displayAmount, ui_prepro_displayAmount);
        *flags |= IO_ASYNCH_REPLY;
    } else {
        display_error();
    }
}

#endif
