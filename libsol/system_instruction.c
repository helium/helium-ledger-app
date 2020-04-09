#include "common_byte_strings.h"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
#include "system_instruction.h"
#include "util.h"
#include <string.h>

const Pubkey system_program_id = {{
    PROGRAM_ID_SYSTEM
}};

static int parse_system_instruction_kind(Parser* parser, enum SystemInstructionKind* kind) {
    uint32_t maybe_kind;
    BAIL_IF(parse_u32(parser, &maybe_kind));
    switch (maybe_kind) {
        case SystemCreateAccount:
        case SystemAssign:
        case SystemTransfer:
        case SystemCreateAccountWithSeed:
        case SystemAdvanceNonceAccount:
        case SystemWithdrawNonceAccount:
        case SystemInitializeNonceAccount:
        case SystemAuthorizeNonceAccount:
        case SystemAllocate:
        case SystemAllocateWithSeed:
        case SystemAssignWithSeed:
            *kind = (enum SystemInstructionKind) maybe_kind;
            return 0;
    }
    return 1;
}

// Returns 0 and populates SystemTransferInfo if provided a MessageHeader and a transfer
// instruction, otherwise non-zero.
static int parse_system_transfer_instruction(Parser* parser, const Instruction* instruction, const MessageHeader* header, SystemTransferInfo* info) {
    BAIL_IF(parse_u64(parser, &info->lamports));
    size_t pubkeys_length = header->pubkeys_header.pubkeys_length;

    BAIL_IF(instruction->accounts_length < 2);
    uint8_t from_index = instruction->accounts[0];
    BAIL_IF(from_index >= pubkeys_length);
    info->from = &header->pubkeys[from_index];

    uint8_t to_index = instruction->accounts[1];
    BAIL_IF(to_index >= pubkeys_length);
    info->to = &header->pubkeys[to_index];

    return 0;
}

static int parse_system_create_account_with_seed_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SystemCreateAccountWithSeedInfo* info
) {
    BAIL_IF(instruction->accounts_length < 2);
    size_t accounts_index = 0;
    size_t pubkeys_index = instruction->accounts[accounts_index++];

    info->from = &header->pubkeys[pubkeys_index++];
    info->to = &header->pubkeys[pubkeys_index++];

    BAIL_IF(parse_pubkey(parser, &info->base));
    BAIL_IF(parse_sized_string(parser, &info->seed));
    BAIL_IF(parse_u64(parser, &info->lamports));

    return 0;
}

static int parse_system_advance_nonce_account_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SystemAdvanceNonceInfo* info
) {
    BAIL_IF(instruction->accounts_length < 3);
    size_t accounts_index = 0;
    size_t pubkeys_index = instruction->accounts[accounts_index++];
    info->account = &header->pubkeys[pubkeys_index];

    accounts_index++; // Skip recent blockhashes sysvar

    pubkeys_index = instruction->accounts[accounts_index++];
    info->authority = &header->pubkeys[pubkeys_index];

    return 0;
}

int parse_system_instructions(const Instruction* instruction, const MessageHeader* header, SystemInfo* info) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_system_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case SystemTransfer:
            return parse_system_transfer_instruction(&parser, instruction, header, &info->transfer);
        case SystemAdvanceNonceAccount:
            return parse_system_advance_nonce_account_instruction(
                &parser,
                instruction,
                header,
                &info->advance_nonce
            );
        case SystemCreateAccountWithSeed:
            return parse_system_create_account_with_seed_instruction(
                &parser,
                instruction,
                header,
                &info->create_account_with_seed
            );
        case SystemCreateAccount:
        case SystemAssign:
        case SystemWithdrawNonceAccount:
        case SystemInitializeNonceAccount:
        case SystemAuthorizeNonceAccount:
        case SystemAllocate:
        case SystemAllocateWithSeed:
        case SystemAssignWithSeed:
            break;
    }

    return 1;
}

static int print_system_transfer_info(const SystemTransferInfo* info, const MessageHeader* header) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_amount(item, "Transfer", info->lamports);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Sender", info->from);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Recipient", info->to);

    item = transaction_summary_fee_payer_item();
    if (memcmp(&header->pubkeys[0], info->to, PUBKEY_SIZE) == 0) {
        transaction_summary_set_fee_payer_string("recipient");
    } else if (memcmp(&header->pubkeys[0], info->from, PUBKEY_SIZE) == 0) {
        transaction_summary_set_fee_payer_string("sender");
    }

    return 0;
}

static int print_system_advance_nonce_account(const SystemAdvanceNonceInfo* info, const MessageHeader* header) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Advance Nonce", info->account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Authorized by", info->authority);

    item = transaction_summary_fee_payer_item();
    if (memcmp(&header->pubkeys[0], info->authority, PUBKEY_SIZE) == 0) {
        transaction_summary_set_fee_payer_string("authority");
    }

    return 0;
}

int print_system_info(const SystemInfo* info, const MessageHeader* header) {
    switch (info->kind) {
        case SystemTransfer:
            return print_system_transfer_info(&info->transfer, header);
        case SystemAdvanceNonceAccount:
            return print_system_advance_nonce_account(&info->advance_nonce, header);
        case SystemCreateAccount:
        case SystemAssign:
        case SystemCreateAccountWithSeed:
        case SystemWithdrawNonceAccount:
        case SystemInitializeNonceAccount:
        case SystemAuthorizeNonceAccount:
        case SystemAllocate:
        case SystemAllocateWithSeed:
        case SystemAssignWithSeed:
            break;
    }

    return 1;
}

int print_system_nonced_transaction_sentinel(const SystemInfo* info, const MessageHeader* header) {
    const SystemAdvanceNonceInfo* nonce_info = &info->advance_nonce;
    SummaryItem* item;

    item = transaction_summary_nonce_account_item();
    summary_item_set_pubkey(item, "Nonce Account", nonce_info->account);

    item = transaction_summary_nonce_authority_item();
    summary_item_set_pubkey(item, "Nonce Authority", nonce_info->authority);

    return 0;
}
