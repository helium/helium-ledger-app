#include "getPubkey.h"
#include "os.h"
#include "ux.h"
#include "cx.h"
#include "menu.h"
#include "utils.h"
#include "sol/parser.h"
#include "sol/printer.h"
#include "sol/print_config.h"
#include "sol/message.h"
#include "sol/transaction_summary.h"

extern uint8_t G_message[MAX_MESSAGE_LENGTH];
static int G_messageLength;
extern uint8_t G_numDerivationPaths;
static uint32_t G_derivationPath[BIP32_PATH];
static int G_derivationPathLength;

/**
 * Checks if data is in UTF-8 format.
 * Adapted from: https://www.cl.cam.ac.uk/~mgk25/ucs/utf8_check.c
 */
bool is_data_utf8(const uint8_t *data, size_t length) {
    if (!data) {
        return false;
    }
    size_t i = 0;
    while (i < length) {
        if (data[i] < 0x80) {
            /* 0xxxxxxx */
            ++i;
        } else if ((data[i] & 0xe0) == 0xc0) {
            /* 110XXXXx 10xxxxxx */
            if (i + 1 >= length || (data[i + 1] & 0xc0) != 0x80 ||
                (data[i] & 0xfe) == 0xc0) /* overlong? */ {
                return false;
            } else {
                i += 2;
            }
        } else if ((data[i] & 0xf0) == 0xe0) {
            /* 1110XXXX 10Xxxxxx 10xxxxxx */
            if (i + 2 >= length || (data[i + 1] & 0xc0) != 0x80 || (data[i + 2] & 0xc0) != 0x80 ||
                (data[i] == 0xe0 && (data[i + 1] & 0xe0) == 0x80) || /* overlong? */
                (data[i] == 0xed && (data[i + 1] & 0xe0) == 0xa0) || /* surrogate? */
                (data[i] == 0xef && data[i + 1] == 0xbf &&
                 (data[i + 2] & 0xfe) == 0xbe)) /* U+FFFE or U+FFFF? */ {
                return false;
            } else {
                i += 3;
            }
        } else if ((data[i] & 0xf8) == 0xf0) {
            /* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
            if (i + 3 >= length || (data[i + 1] & 0xc0) != 0x80 || (data[i + 2] & 0xc0) != 0x80 ||
                (data[i + 3] & 0xc0) != 0x80 ||
                (data[i] == 0xf0 && (data[i + 1] & 0xf0) == 0x80) || /* overlong? */
                (data[i] == 0xf4 && data[i + 1] > 0x8f) || data[i] > 0xf4) /* > U+10FFFF? */ {
                return false;
            } else {
                i += 4;
            }
        } else {
            return false;
        }
    }
    return true;
}

/*
 * Checks if data is in ASCII format
 */
static bool is_data_ascii(const uint8_t *data, size_t length) {
    if (!data) {
        return false;
    }
    for (size_t i = 0; i < length; ++i) {
        if (data[i] < 0x20 || data[i] > 0x7e) {
            return false;
        }
    }
    return true;
}

static void derive_private_key(cx_ecfp_private_key_t *privateKey,
                               uint32_t *derivationPath,
                               uint8_t derivationPathLength) {
    uint8_t privateKeyData[32];
    BEGIN_TRY {
        TRY {
            os_perso_derive_node_bip32_seed_key(HDW_ED25519_SLIP10,
                                                CX_CURVE_Ed25519,
                                                derivationPath,
                                                derivationPathLength,
                                                privateKeyData,
                                                NULL,
                                                (unsigned char *) "ed25519 seed",
                                                12);
            cx_ecfp_init_private_key(CX_CURVE_Ed25519, privateKeyData, 32, privateKey);
        }
        FINALLY {
            MEMCLEAR(privateKeyData);
        }
    }
    END_TRY;
}

static uint8_t set_result_sign_message() {
    uint8_t tx = 64;
    uint8_t signature[SIGNATURE_LENGTH];
    cx_ecfp_private_key_t privateKey;
    BEGIN_TRY {
        TRY {
            derive_private_key(&privateKey, G_derivationPath, G_derivationPathLength);
            cx_eddsa_sign(&privateKey,
                          CX_LAST,
                          CX_SHA512,
                          G_message,
                          G_messageLength,
                          NULL,
                          0,
                          signature,
                          SIGNATURE_LENGTH,
                          NULL);
            memcpy(G_io_apdu_buffer, signature, 64);
        }
        FINALLY {
            MEMCLEAR(privateKey);
        }
    }
    END_TRY;
    return tx;
}

//////////////////////////////////////////////////////////////////////

UX_STEP_NOCB(ux_sign_msg_text_step,
             bnnn_paging,
             {
                 .title = "Message",
                 .text = (const char *) G_message + OFFCHAIN_MESSAGE_HEADER_LENGTH,
             });
UX_STEP_VALID(ux_sign_msg_approve_step,
              pb,
              sendResponse(set_result_sign_message(), true),
              {
                  &C_icon_validate_14,
                  "Approve",
              });
UX_STEP_VALID(ux_sign_msg_reject_step,
              pb,
              sendResponse(0, false),
              {
                  &C_icon_crossmark,
                  "Reject",
              });
UX_STEP_NOCB_INIT(ux_sign_msg_summary_step,
                  bnnn_paging,
                  {
                      size_t step_index = G_ux.flow_stack[stack_slot].index;
                      enum DisplayFlags flags = DisplayFlagNone;
                      if (N_storage.settings.pubkey_display == PubkeyDisplayLong) {
                          flags |= DisplayFlagLongPubkeys;
                      }
                      if (transaction_summary_display_item(step_index, flags)) {
                          THROW(ApduReplySolanaSummaryUpdateFailed);
                      }
                  },
                  {
                      .title = G_transaction_summary_title,
                      .text = G_transaction_summary_text,
                  });

/*
UX Steps:
- Sign Message

if expert mode:
- Version
- Format
- Size
- Hash
- Signer
else if utf8:
- Hash

if ascii:
- message text
*/
static ux_flow_step_t const *flow_steps[8];

void handleSignOffchainMessage(uint8_t p1,
                               uint8_t p2,
                               uint8_t *dataBuffer,
                               uint16_t dataLength,
                               volatile unsigned int *flags,
                               volatile unsigned int *tx) {
    UNUSED(tx);

    if (dataLength == 0) {
        THROW(ApduReplySolanaInvalidMessage);
    }

    if ((p2 & P2_EXTEND) == 0) {
        MEMCLEAR(G_derivationPath);
        MEMCLEAR(G_message);
        G_messageLength = 0;
        G_numDerivationPaths = 1;

        G_numDerivationPaths = dataBuffer[0];
        dataBuffer++;
        dataLength--;
        // We only support one derivation path ATM
        if (G_numDerivationPaths != 1) {
            THROW(ApduReplySdkExceptionOverflow);
        }

        G_derivationPathLength = read_derivation_path(dataBuffer, dataLength, G_derivationPath);
        dataBuffer += 1 + G_derivationPathLength * 4;
        dataLength -= 1 + G_derivationPathLength * 4;
    } else {
        // P2_EXTEND is set to signal that this APDU buffer extends, rather
        // than replaces, the current message buffer. Asserting it with the
        // first APDU buffer is an error, since we haven't yet received a
        // derivation path.
        if (G_numDerivationPaths == 0) {
            THROW(ApduReplySolanaInvalidMessage);
        }
    }

    int messageLength = dataLength;

    if (G_messageLength + messageLength > MAX_MESSAGE_LENGTH) {
        THROW(ApduReplySdkExceptionOverflow);
    }
    memcpy(G_message + G_messageLength, dataBuffer, messageLength);
    G_messageLength += messageLength;

    if (p2 & P2_MORE) {
        THROW(ApduReplySuccess);
    }

    if (p1 == P1_NON_CONFIRM) {
        // Uncomment this to allow blind signing.
        //*tx = set_result_sign_message();
        // THROW(ApduReplySuccess);

        sendResponse(0, false);
    }

    // Host has signaled that the message is complete. We won't be receiving
    // any more extending APDU buffers. Clear the derivation path count so we
    // can detect P2_EXTEND misuse at the start of the next exchange
    G_numDerivationPaths = 0;

    // parse header
    Parser parser = {G_message, G_messageLength};
    struct OffchainMessageHeader header;
    if (parse_offchain_message_header(&parser, &header)) {
        PRINTF("%u %u %u\n", header.version, header.format, header.length);
        THROW(ApduReplySolanaInvalidMessageHeader);
    }

    // validate message
    if (header.version != 0 || header.format > 1 || header.length > MAX_OFFCHAIN_MESSAGE_LENGTH ||
        header.length + OFFCHAIN_MESSAGE_HEADER_LENGTH != G_messageLength) {
        THROW(ApduReplySolanaInvalidMessageHeader);
    }
    const bool is_ascii = is_data_ascii(G_message + OFFCHAIN_MESSAGE_HEADER_LENGTH, header.length);
    const bool is_utf8 =
        is_ascii ? true : is_data_utf8(G_message + OFFCHAIN_MESSAGE_HEADER_LENGTH, header.length);
    PRINTF("%d %d\n", is_ascii, is_utf8);
    if (!is_ascii && (!is_utf8 || header.format == 0)) {
        THROW(ApduReplySolanaInvalidMessageFormat);
    } else if (!is_ascii && N_storage.settings.allow_blind_sign != BlindSignEnabled) {
        THROW(ApduReplySdkNotSupported);
    }

    // compute message hash if needed
    Hash messageHash;
    if (!is_ascii || N_storage.settings.display_mode == DisplayModeExpert) {
        cx_hash_sha256(G_message, G_messageLength, (uint8_t *) &messageHash, HASH_LENGTH);
    }

    // fill out UX steps
    transaction_summary_reset();
    SummaryItem *item = transaction_summary_primary_item();
    summary_item_set_string(item, "Sign", "Off-Chain Message");

    if (N_storage.settings.display_mode == DisplayModeExpert) {
        summary_item_set_u64(transaction_summary_general_item(), "Version", header.version);
        summary_item_set_u64(transaction_summary_general_item(), "Format", header.format);
        summary_item_set_u64(transaction_summary_general_item(), "Size", header.length);
        summary_item_set_hash(transaction_summary_general_item(), "Hash", &messageHash);

        Pubkey signer_pubkey;
        getPublicKey(G_derivationPath, signer_pubkey.data, G_derivationPathLength);
        summary_item_set_pubkey(transaction_summary_general_item(), "Signer", &signer_pubkey);
    } else if (!is_ascii) {
        summary_item_set_hash(transaction_summary_general_item(), "Hash", &messageHash);
    }

    enum SummaryItemKind summary_step_kinds[MAX_TRANSACTION_SUMMARY_ITEMS];
    size_t num_flow_steps = 0;
    size_t num_summary_steps = 0;
    if (transaction_summary_finalize(summary_step_kinds, &num_summary_steps)) {
        THROW(ApduReplySolanaSummaryFinalizeFailed);
    }
    for (size_t i = 0; i < num_summary_steps; i++) {
        flow_steps[num_flow_steps++] = &ux_sign_msg_summary_step;
    }

    if (is_ascii) {
        flow_steps[num_flow_steps++] = &ux_sign_msg_text_step;
    }
    flow_steps[num_flow_steps++] = &ux_sign_msg_approve_step;
    flow_steps[num_flow_steps++] = &ux_sign_msg_reject_step;
    flow_steps[num_flow_steps++] = FLOW_END_STEP;

    ux_flow_init(0, flow_steps, NULL);

    *flags |= IO_ASYNCH_REPLY;
}
