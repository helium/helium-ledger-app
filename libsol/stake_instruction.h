#pragma once

#include "sol/parser.h"
#include "sol/printer.h"

extern const Pubkey stake_program_id;

enum StakeInstructionKind {
    StakeInitialize,
    StakeAuthorize,
    StakeDelegate,
    StakeSplit,
    StakeWithdraw,
    StakeDeactivate,
    StakeSetLockup,
};

typedef struct StakeDelegateInfo {
    Pubkey* stake_pubkey;
    Pubkey* vote_pubkey;
    Pubkey* authorized_pubkey;
} StakeDelegateInfo;

typedef struct StakeInfo {
    enum StakeInstructionKind kind;
    union {
        StakeDelegateInfo delegate_stake;
    };
} StakeInfo;

int parse_stake_instructions(Instruction* instruction, MessageHeader* header, StakeInfo* info);
int print_stake_info(StakeInfo* info, MessageHeader* header);
