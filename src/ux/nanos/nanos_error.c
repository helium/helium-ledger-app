#include "bolos_target.h"

#if defined(TARGET_NANOS) && !defined(HAVE_UX_FLOW)

#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include <cx.h>
#include "helium.h"
#include "helium_ux.h"

static const bagl_element_t ui_displayError[5] = {
        UI_BACKGROUND(),
        UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
        UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
        UI_TEXT(0x00, 0, 12, 128, "ADPU Payload Error"),
        // The visible portion of the public key or address.
        UI_TEXT(0x00, 0, 26, 128, "wrong size!"),
};

static const bagl_element_t* ui_prepro_displayError(const bagl_element_t *element) {
    return element;
}

// Define the button handler for the comparison screen. This doesn't do much.
static unsigned int ui_displayError_button(unsigned int button_mask, __attribute__((unused)) unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_LEFT:
	case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
		break;
	case BUTTON_RIGHT:
	case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
		break;
	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // PROCEED
		// The user has acknowledged the error
		ui_idle();
		break;
	}
	return 0;
}

void display_error() {
    UX_DISPLAY(ui_displayError, ui_prepro_displayError);
}

#endif