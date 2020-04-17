#include "instruction.h"
#include "sol/parser.h"
#include "stake_instruction.h"
#include "system_instruction.h"
#include "util.h"
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

void test_static_brief_initializer_macros() {
    InstructionBrief system_test = SYSTEM_IX_BRIEF(SystemTransfer);
    InstructionBrief system_expect = {
        ProgramIdSystem,
        .system = SystemTransfer
    };
    assert(
        memcmp(&system_test, &system_expect, sizeof(InstructionBrief)) == 0
    );
    InstructionBrief stake_test = STAKE_IX_BRIEF(StakeDelegate);
    InstructionBrief stake_expect = { ProgramIdStake, .stake = StakeDelegate };
    assert(memcmp(&stake_test, &stake_expect, sizeof(InstructionBrief)) == 0);
}

void test_instruction_info_matches_brief() {
    InstructionInfo info = {
        .kind = ProgramIdSystem,
        .system = {
            .kind = SystemTransfer,
            .transfer = { NULL, NULL, 0 },
        },
    };
    InstructionBrief brief_pass = SYSTEM_IX_BRIEF(SystemTransfer);
    assert(instruction_info_matches_brief(&info, &brief_pass));
    InstructionBrief brief_fail = SYSTEM_IX_BRIEF(SystemAdvanceNonceAccount);
    assert(!instruction_info_matches_brief(&info, &brief_fail));
}

void test_instruction_infos_match_briefs() {
    InstructionInfo infos[] = {
        {
            .kind = ProgramIdSystem,
            .system = {
                .kind = SystemTransfer,
                .transfer = { NULL, NULL, 0 },
            },
        },
        {
            .kind = ProgramIdStake,
            .stake = {
                .kind = StakeDelegate,
                .delegate_stake = { NULL, NULL, NULL },
            },
        }
    };
    InstructionBrief briefs[] = {
        SYSTEM_IX_BRIEF(SystemTransfer),
        STAKE_IX_BRIEF(StakeDelegate),
    };
    InstructionBrief bad_briefs[] = {
        SYSTEM_IX_BRIEF(SystemTransfer),
        STAKE_IX_BRIEF(StakeSplit),
    };
    size_t infos_len = ARRAY_LEN(infos);
    assert(infos_len == ARRAY_LEN(briefs));
    assert(infos_len == ARRAY_LEN(bad_briefs));
    assert(instruction_infos_match_briefs(infos, briefs, infos_len));
    assert(!instruction_infos_match_briefs(infos, bad_briefs, infos_len));
}

int main() {
    test_instruction_validate_ok();
    test_instruction_validate_bad_program_id_index_fail();
    test_instruction_validate_bad_first_account_index_fail();
    test_instruction_validate_bad_last_account_index_fail();
    test_instruction_program_id_unknown();
    test_instruction_program_id_stake();
    test_instruction_program_id_system();
    test_static_brief_initializer_macros();
    test_instruction_info_matches_brief();
    test_instruction_infos_match_briefs();

    printf("passed\n");
    return 0;
}
