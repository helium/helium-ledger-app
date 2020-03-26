#include "instruction.h"
#include "stake_instruction.c"
#include <stdio.h>
#include <assert.h>

void test_parse_delegate_stake_instructions() {
     uint8_t message[] = {1, 1, 5, 7, 204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54, 171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 167, 213, 23, 24, 199, 116, 201, 40, 86, 99, 152, 105, 29, 94, 182, 139, 94, 184, 163, 155, 75, 109, 92, 115, 85, 91, 33, 0, 0, 0, 0, 6, 167, 213, 23, 25, 53, 132, 208, 254, 237, 155, 179, 67, 29, 19, 32, 107, 229, 68, 40, 27, 87, 184, 86, 108, 197, 55, 95, 244, 0, 0, 0, 6, 161, 216, 23, 165, 2, 5, 11, 104, 7, 145, 230, 206, 109, 184, 142, 30, 91, 113, 80, 246, 31, 198, 121, 10, 78, 180, 209, 0, 0, 0, 0, 6, 161, 216, 23, 145, 55, 84, 42, 152, 52, 55, 189, 254, 42, 122, 178, 85, 127, 83, 92, 138, 120, 114, 43, 104, 164, 157, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 6, 1, 2, 3, 4, 5, 0, 4, 2, 0, 0, 0};
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0);
    assert(instruction_validate(&instruction, &header) == 0);

    StakeInfo info;
    assert(parse_stake_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);
}

void test_parse_stake_instruction_kind() {
    enum StakeInstructionKind kind;
    uint8_t buf[] = {0, 0, 0, 0};
    Parser parser = {buf, ARRAY_LEN(buf)};
    assert(parse_stake_instruction_kind(&parser, &kind) == 0);
    assert(kind == Initialize);

    buf[0] = 1;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 0);
    assert(kind == Authorize);

    buf[0] = 2;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 0);
    assert(kind == DelegateStake);

    buf[0] = 3;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 0);
    assert(kind == Split);

    buf[0] = 4;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 0);
    assert(kind == Withdraw);

    buf[0] = 5;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 0);
    assert(kind == Deactivate);

    buf[0] = 6;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 0);
    assert(kind == SetLockup);

    // Fail the first unused enum value to be sure this test gets updated
    buf[0] = 7;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 1);

    // Should always fail
    buf[0] = 255;
    buf[1] = 255;
    buf[2] = 255;
    buf[3] = 255;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_stake_instruction_kind(&parser, &kind) == 1);
}

int main() {
    test_parse_delegate_stake_instructions();

    printf("passed\n");
    return 0;
}
