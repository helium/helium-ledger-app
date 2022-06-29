#include "derivation_path.h"
#include "getPubkey.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "sol/printer.h"

static uint8_t publicKey[PUBKEY_LENGTH];
static char publicKeyStr[BASE58_PUBKEY_LENGTH];

static uint8_t set_result_get_pubkey() {
    uint8_t tx = 32;

    memcpy(G_io_apdu_buffer, publicKey, 32);
    return tx;
}

//////////////////////////////////////////////////////////////////////

UX_STEP_NOCB(ux_display_public_flow_5_step,
             bnnn_paging,
             {
                 .title = "Pubkey",
                 .text = publicKeyStr,
             });
UX_STEP_VALID(ux_display_public_flow_6_step,
              pb,
              sendResponse(set_result_get_pubkey(), true),
              {
                  &C_icon_validate_14,
                  "Approve",
              });
UX_STEP_VALID(ux_display_public_flow_7_step,
              pb,
              sendResponse(0, false),
              {
                  &C_icon_crossmark,
                  "Reject",
              });

UX_FLOW(ux_display_public_flow,
        &ux_display_public_flow_5_step,
        &ux_display_public_flow_6_step,
        &ux_display_public_flow_7_step);

void handleGetPubkey(uint8_t p1,
                     uint8_t p2,
                     uint8_t *dataBuffer,
                     uint16_t dataLength,
                     volatile unsigned int *flags,
                     volatile unsigned int *tx) {
    UNUSED(p2);

    uint32_t derivationPath[MAX_BIP32_PATH_LENGTH];
    uint32_t pathLength = read_derivation_path(dataBuffer, dataLength, derivationPath);

    get_public_key(publicKey, derivationPath, pathLength);
    encode_base58(publicKey, PUBKEY_LENGTH, publicKeyStr, BASE58_PUBKEY_LENGTH);

    if (p1 == P1_NON_CONFIRM) {
        *tx = set_result_get_pubkey();
        THROW(ApduReplySuccess);
    } else {
        ux_flow_init(0, ux_display_public_flow, NULL);
        *flags |= IO_ASYNCH_REPLY;
    }
}
