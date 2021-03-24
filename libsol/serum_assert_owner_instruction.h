#pragma once

#include "sol/parser.h"

struct Instruction;

extern const Pubkey serum_assert_owner_program_id;

typedef struct SerumAssertOwnerCheckInfo {
    const Pubkey* account;
    const Pubkey* expected_owner;
} SerumAssertOwnerCheckInfo;

typedef struct SerumAssertOwnerInfo {
    union {
        SerumAssertOwnerCheckInfo check;
    };
} SerumAssertOwnerInfo;

int parse_serum_assert_owner_instructions (
    const Instruction* instruction,
    const MessageHeader* header,
    SerumAssertOwnerInfo* info
);
