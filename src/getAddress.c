#include "getAddress.h"
#include "os.h"
#include "utils.h"

static char address[FULL_ADDRESS_LENGTH];

static uint8_t set_result_get_address() {
    uint8_t tx = 0;
    const uint8_t address_size = strlen(address);
    G_io_apdu_buffer[tx++] = address_size;
    os_memmove(G_io_apdu_buffer + tx, address, address_size);
    tx += address_size;
    return tx;
}

#if defined(TARGET_NANOS)

static const bagl_element_t ui_address_nanos[] = {
    UI_BUTTONS,
    UI_LABELINE(0x01, "Confirm", UI_FIRST,  BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0),
    UI_LABELINE(0x01, "address", UI_SECOND, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0),
    UI_LABELINE(0x02, "Address", UI_FIRST,  BAGL_FONT_OPEN_SANS_REGULAR_11px,   0),
    UI_LABELINE(0x02, address,   UI_SECOND, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 26),
};

static unsigned int ui_address_nanos_button(unsigned int button_mask, unsigned int button_mask_counter) {
    switch(button_mask) {
        case BUTTON_EVT_RELEASED|BUTTON_LEFT: // CANCEL
            sendResponse(0, false);
            break;

        case BUTTON_EVT_RELEASED|BUTTON_RIGHT: // OK
            sendResponse(set_result_get_address(), true);
            break;
    }
    return 0;
}

#elif defined(TARGET_NANOX)
//////////////////////////////////////////////////////////////////////

UX_STEP_NOCB(
    ux_display_public_flow_5_step, 
    bnnn_paging, 
    {
      .title = "Address",
      .text = address,
    });
UX_STEP_VALID(
    ux_display_public_flow_6_step, 
    pb, 
    sendResponse(set_result_get_address(), true),
    {
      &C_icon_validate_14,
      "Approve",
    });
UX_STEP_VALID(
    ux_display_public_flow_7_step, 
    pb, 
    sendResponse(0, false),
    {
      &C_icon_crossmark,
      "Reject",
    });


UX_FLOW(ux_display_public_flow,
  &ux_display_public_flow_5_step,
  &ux_display_public_flow_6_step,
  &ux_display_public_flow_7_step
);

#endif

void handleGetAddress(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
    UNUSED(dataLength);
    UNUSED(p2);
    uint8_t publicKey[32];

    getPublicKey(readUint32BE(dataBuffer), publicKey);

    getAddressStringFromBinary(publicKey, address);


    if (p1 == P1_NON_CONFIRM) {
        *tx = set_result_get_address();
        THROW(0x9000);
    } else {
#if defined(TARGET_NANOS)
        ux_step = 0;
        ux_step_count = 2;
        UX_DISPLAY(ui_address_nanos, ui_prepro);
#elif defined(TARGET_NANOX)
        ux_flow_init(0, ux_display_public_flow, NULL);
#endif
        *flags |= IO_ASYNCH_REPLY;
    }
}
