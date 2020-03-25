#include "instruction.h"
#include "sol/parser.h"
#include "stake_instruction.h"
#include "system_instruction.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_instruction_program_id_system() {
    Pubkey program_id;
    memcpy(&program_id, &system_program_id, PUBKEY_SIZE);
    Instruction instruction = { 0, NULL, 0, NULL, 0 };
    MessageHeader header = {{0, 0, 0, 1}, &program_id, NULL, 1};
    assert(instruction_program_id(&instruction, &header) == ProgramIdSystem);
}

void test_instruction_program_id_stake() {
    Pubkey program_id;
    memcpy(&program_id, &stake_program_id, PUBKEY_SIZE);
    Instruction instruction = { 0, NULL, 0, NULL, 0 };
    MessageHeader header = {{0, 0, 0, 1}, &program_id, NULL, 1};
    assert(instruction_program_id(&instruction, &header) == ProgramIdStake);
}

void test_instruction_program_id_unknown() {
    Pubkey program_id;
    memset(&program_id, 0x01, PUBKEY_SIZE);
    Instruction instruction = { 0, NULL, 0, NULL, 0 };
    MessageHeader header = {{0, 0, 0, 1}, &program_id, NULL, 1};
    assert(instruction_program_id(&instruction, &header) == ProgramIdUnknown);
}

void test_instruction_validate_ok() {
    uint8_t accounts[] = {1, 2, 3};
    Instruction instruction = {0, accounts, 3, NULL, 0};
    MessageHeader header = {{0, 0, 0, 4}, NULL, NULL, 1};
    assert(instruction_validate(&instruction, &header) == 0);
}

void test_instruction_validate_bad_program_id_index_fail() {
    uint8_t accounts[] = {1, 2, 3};
    Instruction instruction = {4, accounts, 3, NULL, 0};
    MessageHeader header = {{0, 0, 0, 4}, NULL, NULL, 1};
    assert(instruction_validate(&instruction, &header) == 1);
}

void test_instruction_validate_bad_first_account_index_fail() {
    uint8_t accounts[] = {4, 2, 3};
    Instruction instruction = {0, accounts, 3, NULL, 0};
    MessageHeader header = {{0, 0, 0, 4}, NULL, NULL, 1};
    assert(instruction_validate(&instruction, &header) == 1);
}

void test_instruction_validate_bad_last_account_index_fail() {
    uint8_t accounts[] = {1, 2, 4};
    Instruction instruction = {0, accounts, 3, NULL, 0};
    MessageHeader header = {{0, 0, 0, 4}, NULL, NULL, 1};
    assert(instruction_validate(&instruction, &header) == 1);
}

int main() {
    test_instruction_validate_ok();
    test_instruction_validate_bad_program_id_index_fail();
    test_instruction_validate_bad_first_account_index_fail();
    test_instruction_validate_bad_last_account_index_fail();
    test_instruction_program_id_unknown();
    test_instruction_program_id_stake();
    test_instruction_program_id_system();

    printf("passed\n");
    return 0;
}
