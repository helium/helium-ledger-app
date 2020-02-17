#include "getPubkey.h"
#include "os.h"
#include "ux.h"
#include "cx.h"
#include "utils.h"
#include "parser.h"
#include "printer.h"
#include "system_instruction.h"

static char messageHash[BASE58_HASH_LENGTH];
static uint8_t signature[SIGNATURE_LENGTH];
static char feePayer[BASE58_PUBKEY_LENGTH];
static char senderPubkey[BASE58_PUBKEY_LENGTH];
static char recipientPubkey[BASE58_PUBKEY_LENGTH];

#define MESSAGE_TRANSFER_SIZE 32
static char messageTransfer[MESSAGE_TRANSFER_SIZE];

#define SUMMARY_LENGTH 7

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
UX_STEP_NOCB(
    ux_transfer_message_flow_0_step,
    bnnn_paging,
    {
      .title = "Transfer",
      .text = messageTransfer,
    });
UX_STEP_NOCB(
    ux_transfer_message_flow_1_step,
    bnnn_paging,
    {
      .title = "Sender",
      .text = senderPubkey,
    });
UX_STEP_NOCB(
    ux_transfer_message_flow_2_step,
    bnnn_paging,
    {
      .title = "Recipient",
      .text = recipientPubkey,
    });
UX_STEP_NOCB(
    ux_fee_payer_step,
    bnnn_paging,
    {
      .title = "Fee paid by",
      .text = feePayer,
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
    ux_display_message_flow_2_step,
    pb,
    sendResponse(0, false),
    {
      &C_icon_crossmark,
      "Reject",
    });

UX_FLOW(ux_display_message,
  &ux_display_message_flow_0_step,
  &ux_fee_payer_step,
  &ux_display_message_flow_1_step,
  &ux_display_message_flow_2_step
);

UX_FLOW(ux_transfer_message,
  &ux_transfer_message_flow_0_step,
  &ux_transfer_message_flow_1_step,
  &ux_transfer_message_flow_2_step,
  &ux_fee_payer_step,
  &ux_display_message_flow_1_step,
  &ux_display_message_flow_2_step
);

void handleSignMessage(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
    UNUSED(p2);
    uint32_t derivationPath[BIP32_PATH];
    uint8_t messageHashBytes[HASH_LENGTH];

    int derivationPathLength = read_derivation_path(dataBuffer, dataLength, derivationPath);
    dataBuffer += 1 + derivationPathLength * 4;
    dataLength -= 1 + derivationPathLength * 4;

    cx_ecfp_private_key_t privateKey;
    derive_private_key(&privateKey, derivationPath, derivationPathLength);

    int messageLength = U2BE(dataBuffer, 0);
    dataBuffer += 2;
    uint8_t *message = dataBuffer;

    Parser parser = {message, messageLength};
    MessageHeader header;
    if (parse_message_header(&parser, &header)) {
        // This is not a valid Solana message
        sendResponse(0, false);
        return;
    }

    SystemTransferInfo info;
    int system_transfer_err = parse_system_transfer_instructions(&parser, &header, &info);

    cx_hash_sha256(dataBuffer, dataLength, messageHashBytes, HASH_LENGTH);

    int len = encodeBase58(messageHashBytes, HASH_LENGTH, (uint8_t*) messageHash, BASE58_HASH_LENGTH);
    messageHash[len] = '\0';

    char pubkeyBuffer[BASE58_PUBKEY_LENGTH];
    len = encodeBase58((uint8_t*) &header.pubkeys[0], PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
   pubkeyBuffer[len] = '\0';
   print_summary(pubkeyBuffer, feePayer, SUMMARY_LENGTH, SUMMARY_LENGTH);

    cx_eddsa_sign(&privateKey, CX_LAST, CX_SHA512, message, messageLength, NULL, 0, signature, SIGNATURE_LENGTH, NULL);

    if (p1 == P1_NON_CONFIRM) {
        // Uncomment this to allow blind signing.
        //*tx = set_result_sign_message();
        //THROW(0x9000);

        sendResponse(0, false);
    } else {
        if (system_transfer_err == 0) {
            print_amount(info.lamports, "SOL", messageTransfer); // MESSAGE_TRANSFER_SIZE

            len = encodeBase58((uint8_t*) info.from, PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
            pubkeyBuffer[len] = '\0';
            print_summary(pubkeyBuffer, senderPubkey, SUMMARY_LENGTH, SUMMARY_LENGTH);

            len = encodeBase58((uint8_t*) info.to, PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
            pubkeyBuffer[len] = '\0';
            print_summary(pubkeyBuffer, recipientPubkey, SUMMARY_LENGTH, SUMMARY_LENGTH);

            if (memcmp(&header.pubkeys[0], info.to, PUBKEY_SIZE) == 0) {
                snprintf(feePayer, BASE58_PUBKEY_LENGTH, "recipient");
            }

            if (memcmp(&header.pubkeys[0], info.from, PUBKEY_SIZE) == 0) {
                snprintf(feePayer, BASE58_PUBKEY_LENGTH, "sender");
            }

            ux_flow_init(0, ux_transfer_message, NULL);
        } else {
            ux_flow_init(0, ux_display_message, NULL);
        }
        *flags |= IO_ASYNCH_REPLY;
    }
}
