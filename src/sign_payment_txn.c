#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include "helium.h"
#include "ux.h"

static calcTxnHashContext_t *ctx = &global.calcTxnHashContext;

static const bagl_element_t ui_signTxn_approve[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x00, BAGL_GLYPH_ICON_CROSS),
	UI_ICON_RIGHT(0x00, BAGL_GLYPH_ICON_CHECK),

	UI_TEXT(0x00, 0, 18, 128, "Sign transaction?"),
};

static const bagl_element_t* ui_prepro_signTxn_approve(const bagl_element_t *element) {
	int fullSize = ctx->fullStr_len;
	if ((element->component.userid == 1 && ctx->displayIndex == 0) ||
	    (element->component.userid == 2 && ctx->displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_signTxn_approve_button(unsigned int button_mask, unsigned int button_mask_counter) {
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
		adpu_tx = create_helium_transaction();
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
	UI_TEXT(0x00, 0, 26, 128, global.getPublicKeyContext.partialStr),
};

static const bagl_element_t* ui_prepro_displayFee(const bagl_element_t *element) {
	int fullSize = ctx->fullStr_len;
	if ((element->component.userid == 1 && ctx->displayIndex == 0) ||
	    (element->component.userid == 2 && ctx->displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_displayFee_button(unsigned int button_mask, unsigned int button_mask_counter) {
	int fullSize = ctx->fullStr_len;
	switch (button_mask) {
	case BUTTON_LEFT:
	case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
		if (ctx->displayIndex > 0) {
			ctx->displayIndex--;
		}
		os_memmove(ctx->partialStr, ctx->fullStr+ctx->displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_RIGHT:
	case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
		if (ctx->displayIndex < fullSize-12) {
			ctx->displayIndex++;
		}
		os_memmove(ctx->partialStr, ctx->fullStr+ctx->displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // PROCEED
		// display approval screen
		UX_DISPLAY(ui_signTxn_approve, ui_prepro_signTxn_approve);
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayRecipient[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, "Recipient Address"),
	// The visible portion of the recipient
	UI_TEXT(0x00, 0, 26, 128, global.getPublicKeyContext.partialStr),
};

static const bagl_element_t* ui_prepro_displayRecipient(const bagl_element_t *element) {
	int fullSize = ctx->fullStr_len;
	if ((element->component.userid == 1 && ctx->displayIndex == 0) ||
	    (element->component.userid == 2 && ctx->displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_displayRecipient_button(unsigned int button_mask, unsigned int button_mask_counter) {
	int fullSize = ctx->fullStr_len;
	uint8_t len;
	switch (button_mask) {
	case BUTTON_LEFT:
	case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
		if (ctx->displayIndex > 0) {
			ctx->displayIndex--;
		}
		os_memmove(ctx->partialStr, ctx->fullStr+ctx->displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_RIGHT:
	case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
		if (ctx->displayIndex < fullSize-12) {
			ctx->displayIndex++;
		}
		os_memmove(ctx->partialStr, ctx->fullStr+ctx->displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // PROCEED

		// display data credit transaction fee
		len = bin2dec(ctx->fullStr, ctx->fee);
		ctx->fullStr_len = len;
		ctx->fullStr[len] = '\0';
		
		uint8_t partlen = 12;
		if(len < 12){
			partlen = len;
		}
		os_memmove(ctx->partialStr, ctx->fullStr, partlen);
		ctx->partialStr[partlen] = '\0';
		ctx->displayIndex = 0;

		UX_DISPLAY(ui_displayFee, ui_prepro_displayFee);
		break;
	}
	return 0;
}

static const bagl_element_t ui_displayAmount[] = {
	UI_BACKGROUND(),
	UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
	UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
	UI_TEXT(0x00, 0, 12, 128, "Amount HNT"),
	// The visible portion of the amount
	UI_TEXT(0x00, 0, 26, 128, global.getPublicKeyContext.partialStr),
};

static const bagl_element_t* ui_prepro_displayAmount(const bagl_element_t *element) {
	int fullSize = ctx->fullStr_len;
	if ((element->component.userid == 1 && ctx->displayIndex == 0) ||
	    (element->component.userid == 2 && ctx->displayIndex == fullSize-12)) {
		return NULL;
	}
	return element;
}

static unsigned int ui_displayAmount_button(unsigned int button_mask, unsigned int button_mask_counter) {
	int fullSize = ctx->fullStr_len;
	cx_sha256_t hash;
	unsigned char hash_buffer[32];
	// use the G_io_apdu buffer as a scratchpad to minimize stack usage
	uint8_t * address_with_check = G_io_apdu_buffer;

	switch (button_mask) {
	case BUTTON_LEFT:
	case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
		if (ctx->displayIndex > 0) {
			ctx->displayIndex--;
		}
		os_memmove(ctx->partialStr, ctx->fullStr+ctx->displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_RIGHT:
	case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
		if (ctx->displayIndex < fullSize-12) {
			ctx->displayIndex++;
		}
		os_memmove(ctx->partialStr, ctx->fullStr+ctx->displayIndex, 12);
		UX_REDISPLAY();
		break;

	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // PROCEED
		// display recipient address on screen
		os_memmove(address_with_check, ctx->payee, 34);

		cx_sha256_init(&hash);
		cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
		cx_sha256_init(&hash);
		cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
		os_memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
		size_t output_len;
		btchip_encode_base58(address_with_check, 38, ctx->fullStr, &output_len);
		ctx->fullStr[output_len] = '\0';
		ctx->fullStr_len = output_len;
		os_memmove(ctx->partialStr, ctx->fullStr, 12);
		ctx->partialStr[12] = '\0';
		ctx->displayIndex = 0;

		UX_DISPLAY(ui_displayRecipient, ui_prepro_displayRecipient);
		break;
	}
	return 0;
}

void handle_sign_payment_txn(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
	// copy data out of dataBuffer and into transaction context
	ctx->amount = U8LE(dataBuffer, 0);
	ctx->fee  = U8LE(dataBuffer, 8);
	ctx->nonce = U8LE(dataBuffer, 16);
	os_memmove(ctx->payee, &dataBuffer[24], sizeof(ctx->payee));

	// display amount on screen
	uint8_t len = pretty_print_hnt(ctx->fullStr, ctx->amount);
	uint8_t i = 0;
	while(ctx->fullStr[i] != '\0' && i<12){
		ctx->partialStr[i] = ctx->fullStr[i];
		i++;
	}
	ctx->partialStr[i] = '\0';
	ctx->fullStr_len = len;

	ctx->displayIndex = 0;

	UX_DISPLAY(ui_displayAmount, ui_prepro_displayAmount);
	*flags |= IO_ASYNCH_REPLY;
}
