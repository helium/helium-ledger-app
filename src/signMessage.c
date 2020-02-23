#include "getPubkey.h"
#include "os.h"
#include "ux.h"
#include "cx.h"
#include "utils.h"
#include "parser.h"
#include "printer.h"
#include "system_instruction.h"
#include "stake_instruction.h"

#define TITLE_SIZE 32
#define SUMMARY_LENGTH 7

typedef struct field_t {
    char title[TITLE_SIZE];
    char text[BASE58_PUBKEY_LENGTH];
} field_t;

static field_t G_fields[4];

static uint8_t G_message[MAX_MESSAGE_LENGTH];
static int G_messageLength;
static uint32_t G_derivationPath[BIP32_PATH];
static int G_derivationPathLength;

void derive_private_key(cx_ecfp_private_key_t *privateKey, uint32_t *derivationPath, uint8_t derivationPathLength) {
    uint8_t privateKeyData[32];
    os_perso_derive_node_bip32_seed_key(HDW_ED25519_SLIP10, CX_CURVE_Ed25519, derivationPath, derivationPathLength, privateKeyData, NULL, (unsigned char*) "ed25519 seed", 12);
    cx_ecfp_init_private_key(CX_CURVE_Ed25519, privateKeyData, 32, privateKey);
    MEMCLEAR(privateKeyData);
}

static uint8_t set_result_sign_message() {
    uint8_t tx = 64;
    uint8_t signature[SIGNATURE_LENGTH];
    cx_ecfp_private_key_t privateKey;
    derive_private_key(&privateKey, G_derivationPath, G_derivationPathLength);
    cx_eddsa_sign(&privateKey, CX_LAST, CX_SHA512, G_message, G_messageLength, NULL, 0, signature, SIGNATURE_LENGTH, NULL);
    os_memmove(G_io_apdu_buffer, signature, 64);
    return tx;
}

//////////////////////////////////////////////////////////////////////

UX_STEP_NOCB(
    ux_instruction_step,
    bnnn_paging,
    {
      .title = G_fields[0].title,
      .text = G_fields[0].text,
    });
UX_STEP_NOCB(
    ux_sender_step,
    bnnn_paging,
    {
      .title = G_fields[1].title,
      .text = G_fields[1].text,
    });
UX_STEP_NOCB(
    ux_recipient_step,
    bnnn_paging,
    {
      .title = G_fields[2].title,
      .text = G_fields[2].text,
    });
UX_STEP_NOCB(
    ux_fee_payer_step,
    bnnn_paging,
    {
      .title = "Fee paid by",
      .text = G_fields[3].text,
    });
UX_STEP_VALID(
    ux_approve_step,
    pb,
    sendResponse(set_result_sign_message(), true),
    {
      &C_icon_validate_14,
      "Approve",
    });
UX_STEP_VALID(
    ux_reject_step,
    pb,
    sendResponse(0, false),
    {
      &C_icon_crossmark,
      "Reject",
    });

UX_FLOW(ux_3_fields,
  &ux_instruction_step,
  &ux_sender_step,
  &ux_fee_payer_step,
  &ux_approve_step,
  &ux_reject_step
);

UX_FLOW(ux_4_fields,
  &ux_instruction_step,
  &ux_sender_step,
  &ux_recipient_step,
  &ux_fee_payer_step,
  &ux_approve_step,
  &ux_reject_step
);

static int process_message_body(uint8_t* message_body, int message_body_length, MessageHeader* header, field_t* fields, size_t* fields_used) {
    Parser parser = {message_body, message_body_length};
    char pubkeyBuffer[BASE58_PUBKEY_LENGTH];

    // Check if these are system instructions
    SystemTransferInfo transfer_info;
    if (parse_system_transfer_instructions(&parser, header, &transfer_info) == 0) {
        strcpy(fields[0].title, "Transfer");
        print_amount(transfer_info.lamports, "SOL", fields[0].text);

        strcpy(fields[1].title, "Sender");
        encode_base58((uint8_t*) transfer_info.from, PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
        print_summary(pubkeyBuffer, fields[1].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

        strcpy(fields[2].title, "Recipient");
        encode_base58((uint8_t*) transfer_info.to, PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
        print_summary(pubkeyBuffer, fields[2].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

        if (memcmp(&header->pubkeys[0], transfer_info.to, PUBKEY_SIZE) == 0) {
            snprintf(fields[3].text, BASE58_PUBKEY_LENGTH, "recipient");
        }

        if (memcmp(&header->pubkeys[0], transfer_info.from, PUBKEY_SIZE) == 0) {
            snprintf(fields[3].text, BASE58_PUBKEY_LENGTH, "sender");
        }

        *fields_used = 4;
        return 0;
    }

    // Reset the parser
    parser.buffer = message_body;
    parser.buffer_length = message_body_length;

    // Check if these are staking instructions
    DelegateStakeInfo delegate_info;
    if (parse_delegate_stake_instructions(&parser, header, &delegate_info) == 0) {
        strcpy(fields[0].title, "Delegate from");
        encode_base58((uint8_t*) delegate_info.stake_pubkey, PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
        print_summary(pubkeyBuffer, fields[0].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

        strcpy(fields[1].title, "Authorized by");
        encode_base58((uint8_t*) delegate_info.authorized_pubkey, PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
        print_summary(pubkeyBuffer, fields[1].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

        strcpy(fields[2].title, "Vote account");
        encode_base58((uint8_t*) delegate_info.vote_pubkey, PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
        print_summary(pubkeyBuffer, fields[2].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

        if (memcmp(&header->pubkeys[0], delegate_info.authorized_pubkey, PUBKEY_SIZE) == 0) {
            snprintf(fields[3].text, BASE58_PUBKEY_LENGTH, "authorizer");
        }

        *fields_used = 4;
        return 0;
    }

    *fields_used = 0;
    return 1;
}

void handleSignMessage(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
    if ((p2 & P2_EXTEND) == 0) {
        MEMCLEAR(G_derivationPath);
        MEMCLEAR(G_message);
	G_messageLength = 0;

        G_derivationPathLength = read_derivation_path(dataBuffer, dataLength, G_derivationPath);
        dataBuffer += 1 + G_derivationPathLength * 4;
        dataLength -= 1 + G_derivationPathLength * 4;
    }

    int messageLength = U2BE(dataBuffer, 0);
    dataBuffer += 2;

    if (G_messageLength + messageLength > MAX_MESSAGE_LENGTH) {
        THROW(EXCEPTION_OVERFLOW);
    }
    os_memmove(G_message + G_messageLength, dataBuffer, messageLength);
    G_messageLength += messageLength;

    if (p2 & P2_MORE) {
        THROW(0x9000);
    }

    Parser parser = {G_message, G_messageLength};
    MessageHeader header;
    if (parse_message_header(&parser, &header)) {
        // This is not a valid Solana message
        sendResponse(0, false);
        return;
    }

    if (p1 == P1_NON_CONFIRM) {
        // Uncomment this to allow blind signing.
        //*tx = set_result_sign_message();
        //THROW(0x9000);

        sendResponse(0, false);
    }

    // Set fee payer text
    char pubkeyBuffer[BASE58_PUBKEY_LENGTH];
    encode_base58((uint8_t*) &header.pubkeys[0], PUBKEY_LENGTH, (uint8_t*) pubkeyBuffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkeyBuffer, G_fields[3].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

    size_t fieldsUsed;
    if (process_message_body(parser.buffer, parser.buffer_length, &header, G_fields, &fieldsUsed)) {
        strcpy(G_fields[0].title, "Unrecognized");
        strcpy(G_fields[0].text, "format");

        uint8_t messageHashBytes[HASH_LENGTH];
        cx_hash_sha256(dataBuffer, dataLength, messageHashBytes, HASH_LENGTH);

        strcpy(G_fields[1].title, "Message Hash");
        encode_base58(messageHashBytes, HASH_LENGTH, (uint8_t*) G_fields[1].text, BASE58_HASH_LENGTH);
        fieldsUsed = 3;
    }

    switch (fieldsUsed) {
    case 3:
        ux_flow_init(0, ux_3_fields, NULL);
        break;
    case 4:
        ux_flow_init(0, ux_4_fields, NULL);
        break;
    }

    *flags |= IO_ASYNCH_REPLY;
}
