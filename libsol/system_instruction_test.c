#include "instruction.h"
#include "system_instruction.c"
#include <stdio.h>
#include <assert.h>

#define assert_string_equal(actual, expected) assert(strcmp(actual, expected) == 0)

void test_parse_system_transfer_instructions() {
    uint8_t message[] = {1, 0, 1, 3, 171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218, 204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0);
    assert(instruction_validate(&instruction, &header) == 0);

    Pubkey* fee_payer_pubkey = &header.pubkeys[0];
    SystemInfo info;
    assert(parse_system_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);
    assert(info.transfer.lamports == 42);
    assert(memcmp(fee_payer_pubkey, info.transfer.from, PUBKEY_SIZE) == 0);
}

void test_parse_system_transfer_instructions_with_payer() {
    uint8_t message[] = {2, 0, 1, 3, 204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54, 171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0);
    assert(instruction_validate(&instruction, &header) == 0);

    Pubkey* fee_payer_pubkey = &header.pubkeys[0];
    SystemInfo info;
    assert(parse_system_instructions(&instruction, &header, &info) == 0);

    // "to", not "from", is paying for this transaction.
    assert(memcmp(fee_payer_pubkey, info.transfer.to, PUBKEY_SIZE) == 0);
}


void test_process_system_transfer() {
    uint8_t message[] = {1, 0, 1, 3, 171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218, 204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0);
    assert(instruction_validate(&instruction, &header) == 0);

    field_t fields[5];
    size_t fields_used;
    SystemInfo info;
    assert(parse_system_instructions(&instruction, &header, &info) == 0);
    assert(print_system_info(&info, &header, fields, &fields_used) == 0);
    assert_string_equal(fields[0].text, "0.000000042 SOL");

    // Fee-payer is sender
    assert_string_equal(fields[3].text, "sender");
}

void test_parse_system_instruction_kind() {
    enum SystemInstructionKind kind;
    uint8_t buf[] = {0, 0, 0, 0};
    Parser parser = {buf, ARRAY_LEN(buf)};
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == CreateAccount);

    buf[0] = 1;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == Assign);

    buf[0] = 2;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == Transfer);

    buf[0] = 3;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == CreateAccountWithSeed);

    buf[0] = 4;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == AdvanceNonceAccount);

    buf[0] = 5;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == WithdrawNonceAccount);

    buf[0] = 6;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == InitializeNonceAccount);

    buf[0] = 7;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == AuthorizeNonceAccount);

    buf[0] = 8;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == Allocate);

    buf[0] = 9;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == AllocateWithSeed);

    buf[0] = 10;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 0);
    assert(kind == AssignWithSeed);

    // Fail the first unused enum value to be sure this test gets updated
    buf[0] = 11;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 1);

    // Should always fail
    buf[0] = 255;
    buf[1] = 255;
    buf[2] = 255;
    buf[3] = 255;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_system_instruction_kind(&parser, &kind) == 1);
}

int main() {
    test_parse_system_transfer_instructions();
    test_parse_system_transfer_instructions_with_payer();
    test_process_system_transfer();

    printf("passed\n");
    return 0;
}
