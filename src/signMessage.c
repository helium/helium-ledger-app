#include "getPubkey.h"
#include "os.h"
#include "ux.h"
#include "cx.h"
#include "utils.h"

static char messageHash[FULL_HASH_LENGTH];
static unsigned char signature[FULL_SIGNATURE_LENGTH];

void derive_private_key(cx_ecfp_private_key_t *privateKey, uint32_t *derivationPath, uint8_t derivationPathLength) {
    uint8_t privateKeyData[32];
    os_perso_derive_node_bip32_seed_key(HDW_ED25519_SLIP10, CX_CURVE_Ed25519, derivationPath, derivationPathLength, privateKeyData, NULL, (unsigned char*) "ed25519 seed", 12);
    cx_ecfp_init_private_key(CX_CURVE_Ed25519, privateKeyData, 32, privateKey);
    MEMCLEAR(privateKeyData);
}

static uint8_t set_result_sign_message() {
    uint8_t tx = 64;
    os_memmove(G_io_apdu_buffer, signature, 64);
    return tx;
}

//////////////////////////////////////////////////////////////////////

UX_STEP_NOCB(
    ux_display_message_flow_0_step,
    bnnn_paging,
    {
      .title = "Message Hash",
      .text = messageHash,
    });
UX_STEP_VALID(
    ux_display_message_flow_1_step,
    pb,
    sendResponse(set_result_sign_message(), true),
    {
      &C_icon_validate_14,
      "Approve",
    });
UX_STEP_VALID(
    ux_diplay_message_flow_2_step,
    pb,
    sendResponse(0, false),
    {
      &C_icon_crossmark,
      "Reject",
    });

UX_FLOW(ux_display_message,
  &ux_display_message_flow_0_step,
  &ux_display_message_flow_1_step,
  &ux_diplay_message_flow_2_step
);

void handleSignMessage(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
    UNUSED(p2);
    uint32_t derivationPath[BIP32_PATH];

    int derivationPathLength = read_derivation_path(dataBuffer, dataLength, derivationPath);
    dataBuffer += 1 + derivationPathLength * 4;
    dataLength -= 1 + derivationPathLength * 4;

    cx_ecfp_private_key_t privateKey;
    derive_private_key(&privateKey, derivationPath, derivationPathLength);

    int messageLength = dataBuffer[0];
    dataBuffer += 1;
    uint8_t *message = dataBuffer;

    cx_hash_sha256(dataBuffer, dataLength, (unsigned char *)messageHash, FULL_HASH_LENGTH);
    cx_eddsa_sign(&privateKey, CX_LAST, CX_SHA512, message, messageLength, NULL, 0, signature, FULL_SIGNATURE_LENGTH, NULL);

    if (p1 == P1_NON_CONFIRM) {
        *tx = set_result_sign_message();
        THROW(0x9000);
    } else {
        ux_flow_init(0, ux_display_message, NULL);
        *flags |= IO_ASYNCH_REPLY;
    }
}
