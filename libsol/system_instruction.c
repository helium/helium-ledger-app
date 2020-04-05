#include "sol/parser.h"
#include "sol/printer.h"
#include "system_instruction.h"
#include "util.h"
#include <string.h>

const Pubkey system_program_id = {{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
}};

static int parse_system_instruction_kind(Parser* parser, enum SystemInstructionKind* kind) {
    uint32_t maybe_kind;
    BAIL_IF(parse_u32(parser, &maybe_kind));
    switch (maybe_kind) {
        case CreateAccount:
        case Assign:
        case Transfer:
        case CreateAccountWithSeed:
        case AdvanceNonceAccount:
        case WithdrawNonceAccount:
        case InitializeNonceAccount:
        case AuthorizeNonceAccount:
        case Allocate:
        case AllocateWithSeed:
        case AssignWithSeed:
            *kind = (enum SystemInstructionKind) maybe_kind;
            return 0;
    }
    return 1;
}

// Returns 0 and populates SystemTransferInfo if provided a MessageHeader and a transfer
// instruction, otherwise non-zero.
static int parse_system_transfer_instruction(Parser* parser, Instruction* instruction, Pubkey* pubkeys, size_t pubkeys_length, SystemTransferInfo* info) {
    BAIL_IF(parse_u64(parser, &info->lamports));

    BAIL_IF(instruction->accounts_length < 2);
    uint8_t from_index = instruction->accounts[0];
    BAIL_IF(from_index >= pubkeys_length);
    info->from = &pubkeys[from_index];

    uint8_t to_index = instruction->accounts[1];
    BAIL_IF(to_index >= pubkeys_length);
    info->to = &pubkeys[to_index];

    return 0;
}

static int parse_system_advance_nonce_account_instruction(
    Parser* parser,
    Instruction* instruction,
    MessageHeader* header,
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

int parse_system_instructions(Instruction* instruction, MessageHeader* header, SystemInfo* info) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_system_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case Transfer:
            return parse_system_transfer_instruction(&parser, instruction, header->pubkeys, header->pubkeys_header.pubkeys_length, &info->transfer);
        case AdvanceNonceAccount:
            return parse_system_advance_nonce_account_instruction(
                &parser,
                instruction,
                header,
                &info->advance_nonce
            );
        case CreateAccount:
        case Assign:
        case CreateAccountWithSeed:
        case WithdrawNonceAccount:
        case InitializeNonceAccount:
        case AuthorizeNonceAccount:
        case Allocate:
        case AllocateWithSeed:
        case AssignWithSeed:
            break;
    }

    return 1;
}

static int print_system_transfer_info(SystemTransferInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used) {
    strcpy(fields[0].title, "Transfer");
    print_amount(info->lamports, "SOL", fields[0].text, BASE58_PUBKEY_LENGTH);

    char pubkey_buffer[BASE58_PUBKEY_LENGTH];
    strcpy(fields[1].title, "Sender");
    encode_base58(info->from, PUBKEY_SIZE, pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[1].text, BASE58_PUBKEY_SHORT, SUMMARY_LENGTH, SUMMARY_LENGTH);

    strcpy(fields[2].title, "Recipient");
    encode_base58(info->to, PUBKEY_SIZE, pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[2].text, BASE58_PUBKEY_SHORT, SUMMARY_LENGTH, SUMMARY_LENGTH);

    if (memcmp(&header->pubkeys[0], info->to, PUBKEY_SIZE) == 0) {
        strcpy(fields[3].text, "recipient");
    }

    if (memcmp(&header->pubkeys[0], info->from, PUBKEY_SIZE) == 0) {
        strcpy(fields[3].text, "sender");
    }

    *fields_used += 4;
    return 0;
}

static int print_system_advance_nonce_account(SystemAdvanceNonceInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used) {
    char pubkey_buffer[BASE58_PUBKEY_LENGTH];

    strcpy(fields[0].title, "Advance Nonce");
    encode_base58(info->account, PUBKEY_SIZE, pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[0].text, BASE58_PUBKEY_SHORT, SUMMARY_LENGTH, SUMMARY_LENGTH);

    strcpy(fields[1].title, "Authorized by");
    encode_base58(info->authority, PUBKEY_SIZE, pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[1].text, BASE58_PUBKEY_SHORT, SUMMARY_LENGTH, SUMMARY_LENGTH);

    if (memcmp(&header->pubkeys[0], info->authority, PUBKEY_SIZE) == 0) {
        strcpy(fields[3].text, "authority");
    } else {
        encode_base58(&header->pubkeys[0], PUBKEY_SIZE, pubkey_buffer, BASE58_PUBKEY_LENGTH);
        print_summary(pubkey_buffer, fields[3].text, BASE58_PUBKEY_SHORT, SUMMARY_LENGTH, SUMMARY_LENGTH);
    }

    *fields_used += 3;
    return 0;
}

int print_system_info(SystemInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used) {
    switch (info->kind) {
        case Transfer:
            return print_system_transfer_info(&info->transfer, header, fields, fields_used);
        case AdvanceNonceAccount:
            return print_system_advance_nonce_account(&info->advance_nonce, header, fields, fields_used);
        case CreateAccount:
        case Assign:
        case CreateAccountWithSeed:
        case WithdrawNonceAccount:
        case InitializeNonceAccount:
        case AuthorizeNonceAccount:
        case Allocate:
        case AllocateWithSeed:
        case AssignWithSeed:
            break;
    }

    return 1;
}

int print_system_nonced_transaction_sentinel(SystemInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used) {
    SystemAdvanceNonceInfo* nonce_info = &info->advance_nonce;
    char pubkey_buffer[BASE58_PUBKEY_LENGTH];

    encode_base58(nonce_info->account, PUBKEY_SIZE, pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[4].text, BASE58_PUBKEY_SHORT, SUMMARY_LENGTH, SUMMARY_LENGTH);

    encode_base58(nonce_info->authority, PUBKEY_SIZE, pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[5].text, BASE58_PUBKEY_SHORT, SUMMARY_LENGTH, SUMMARY_LENGTH);

    *fields_used += 2;
    return 0;
}
