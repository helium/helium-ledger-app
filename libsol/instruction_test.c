#include "instruction.h"
#include "sol/parser.h"
#include <assert.h>

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
}
