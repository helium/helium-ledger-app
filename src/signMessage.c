#include "getPubkey.h"
#include "os.h"
#include "ux.h"
#include "cx.h"
#include "menu.h"
#include "utils.h"
#include "sol/parser.h"
#include "sol/printer.h"
#include "sol/message.h"
#include "sol/transaction_summary.h"

static uint8_t G_message[MAX_MESSAGE_LENGTH];
static int G_messageLength;
static uint8_t G_numDerivationPaths;
static uint32_t G_derivationPath[BIP32_PATH];
static int G_derivationPathLength;

void derive_private_key(
    cx_ecfp_private_key_t *privateKey,
    uint32_t *derivationPath,
    uint8_t derivationPathLength
) {
    uint8_t privateKeyData[32];
    os_perso_derive_node_bip32_seed_key(
        HDW_ED25519_SLIP10,
        CX_CURVE_Ed25519,
        derivationPath,
        derivationPathLength,
        privateKeyData,
        NULL,
        (unsigned char*) "ed25519 seed",
        12
    );
    cx_ecfp_init_private_key(CX_CURVE_Ed25519, privateKeyData, 32, privateKey);
    MEMCLEAR(privateKeyData);
}

static uint8_t set_result_sign_message() {
    uint8_t tx = 64;
    uint8_t signature[SIGNATURE_LENGTH];
    cx_ecfp_private_key_t privateKey;
    derive_private_key(&privateKey, G_derivationPath, G_derivationPathLength);
    cx_eddsa_sign(
        &privateKey,
        CX_LAST,
        CX_SHA512,
        G_message,
        G_messageLength,
        NULL,
        0,
        signature,
        SIGNATURE_LENGTH,
        NULL
    );
    os_memmove(G_io_apdu_buffer, signature, 64);
    return tx;
}

//////////////////////////////////////////////////////////////////////

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
UX_STEP_NOCB_INIT(
    ux_summary_step,
    paging,
    {
        size_t step_index = G_ux.flow_stack[stack_slot].index;
        enum DisplayFlags flags = DisplayFlagNone;
        if (N_storage.settings.pubkey_display == PubkeyDisplayLong) {
            flags |=  DisplayFlagLongPubkeys;
        }
        if (transaction_summary_display_item(step_index, flags)) {
            THROW(0x6f01);
        }
    },
    {
        .title = G_transaction_summary_title,
        .text = G_transaction_summary_text,
    }
);

#define MAX_FLOW_STEPS ( \
    MAX_TRANSACTION_SUMMARY_ITEMS   \
    + 1     /* approve */           \
    + 1     /* reject */            \
    + 1     /* FLOW_END_STEP */     \
)
ux_flow_step_t const * flow_steps[MAX_FLOW_STEPS];

Hash UnrecognizedMessageHash;

void handleSignMessage(
    uint8_t p1,
    uint8_t p2,
    uint8_t *dataBuffer,
    uint16_t dataLength,
    volatile unsigned int *flags,
    volatile unsigned int *tx
) {
    int deprecated_host = ((dataLength & DATA_HAS_LENGTH_PREFIX) != 0);

    if (deprecated_host) {
        dataLength &= ~DATA_HAS_LENGTH_PREFIX;
    }

    if ((p2 & P2_EXTEND) == 0) {
        MEMCLEAR(G_derivationPath);
        MEMCLEAR(G_message);
        G_messageLength = 0;
        G_numDerivationPaths = 0;

        if (!deprecated_host) {
            G_numDerivationPaths = dataBuffer[0];
            dataBuffer++;
            dataLength--;
            // We only support one derivation path ATM
            if (G_numDerivationPaths != 1) {
                THROW(EXCEPTION_OVERFLOW);
            }
        }

        G_derivationPathLength = read_derivation_path(
            dataBuffer,
            dataLength,
            G_derivationPath
        );
        dataBuffer += 1 + G_derivationPathLength * 4;
        dataLength -= 1 + G_derivationPathLength * 4;
    }

    int messageLength;
    if (deprecated_host) {
        messageLength = U2BE(dataBuffer, 0);
        dataBuffer += 2;
    } else {
        messageLength = dataLength;
    }

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
        THROW(0x6a80);
        return;
    }

    if (p1 == P1_NON_CONFIRM) {
        // Uncomment this to allow blind signing.
        //*tx = set_result_sign_message();
        //THROW(0x9000);

        sendResponse(0, false);
    }

    transaction_summary_reset();
    if (process_message_body(parser.buffer, parser.buffer_length, &header)) {
        if (N_storage.settings.allow_blind_sign == BlindSignEnabled) {
            SummaryItem* item = transaction_summary_primary_item();
            summary_item_set_string(item, "Unrecognized", "format");

            cx_hash_sha256(
                dataBuffer,
                dataLength,
                (uint8_t*) &UnrecognizedMessageHash,
                HASH_LENGTH
            );

            item = transaction_summary_general_item();
            summary_item_set_hash(item, "Message Hash", &UnrecognizedMessageHash);
        } else {
            THROW(NOT_SUPPORTED);
        }
    }

    // Set fee-payer if it hasn't already been resolved by
    // the transaction printer
    transaction_summary_set_fee_payer_pubkey(&header.pubkeys[0]);

    enum SummaryItemKind summary_step_kinds[MAX_TRANSACTION_SUMMARY_ITEMS];
    size_t num_summary_steps = 0;
    if (transaction_summary_finalize(
            summary_step_kinds,
            &num_summary_steps
        ) == 0
    ) {
        size_t num_flow_steps = 0;

        for (size_t i = 0; i < num_summary_steps; i++) {
            flow_steps[num_flow_steps++] = &ux_summary_step;
        }

        flow_steps[num_flow_steps++] = &ux_approve_step;
        flow_steps[num_flow_steps++] = &ux_reject_step;
        flow_steps[num_flow_steps++] = FLOW_END_STEP;

        ux_flow_init(0, flow_steps, NULL);
    } else {
        THROW(0x6f00);
        return;
    }

    *flags |= IO_ASYNCH_REPLY;
}
