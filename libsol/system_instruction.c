#include "sol/parser.h"
#include "sol/printer.h"
#include "system_instruction.h"
#include <string.h>

#define BAIL_IF(x) {int err = x; if (err) return err;}

static int parse_system_instruction_kind(Parser* parser, enum SystemInstructionKind* kind) {
    return parse_u32(parser, (uint32_t *) kind);
}

static int parse_system_transfer_instruction(Instruction* instruction, Pubkey* pubkeys, size_t pubkeys_length, SystemTransferInfo* info) {
    Parser parser = {instruction->data, instruction->data_length};

    enum SystemInstructionKind kind;
    BAIL_IF(parse_system_instruction_kind(&parser, &kind));
    BAIL_IF(kind != Transfer);
    BAIL_IF(parse_u64(&parser, &info->lamports));

    BAIL_IF(instruction->accounts_length < 2);
    uint8_t from_index = instruction->accounts[0];
    BAIL_IF(from_index >= pubkeys_length);
    info->from = &pubkeys[from_index];

    uint8_t to_index = instruction->accounts[1];
    BAIL_IF(to_index >= pubkeys_length);
    info->to = &pubkeys[to_index];

    return 0;
}

// Returns 0 and populates SystemTransferInfo if provided a MessageHeader and a transfer
// instruction, otherwise non-zero.
int parse_system_transfer_instructions(Parser* parser, MessageHeader* header, SystemTransferInfo* info) {
    BAIL_IF(header->instructions_length != 1);

    Instruction instruction;
    BAIL_IF(parse_instruction(parser, &instruction));

    Pubkey* program_id = &header->pubkeys[instruction.program_id_index];
    Pubkey system_program_id = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    BAIL_IF(memcmp(program_id, &system_program_id, PUBKEY_SIZE));

    BAIL_IF(parse_system_transfer_instruction(&instruction, header->pubkeys, header->pubkeys_header.pubkeys_length, info));

    return 0;
}

int print_system_transfer_info(SystemTransferInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used) {
    strcpy(fields[0].title, "Transfer");
    print_amount(info->lamports, "SOL", fields[0].text);

    char pubkey_buffer[BASE58_PUBKEY_LENGTH];
    strcpy(fields[1].title, "Sender");
    encode_base58((uint8_t*) info->from, PUBKEY_SIZE, (uint8_t*) pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[1].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

    strcpy(fields[2].title, "Recipient");
    encode_base58((uint8_t*) info->to, PUBKEY_SIZE, (uint8_t*) pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[2].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

    if (memcmp(&header->pubkeys[0], info->to, PUBKEY_SIZE) == 0) {
        strcpy(fields[3].text, "recipient");
    }

    if (memcmp(&header->pubkeys[0], info->from, PUBKEY_SIZE) == 0) {
        strcpy(fields[3].text, "sender");
    }

    *fields_used = 4;
    return 0;
}

