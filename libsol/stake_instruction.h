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

// To support the `LockupArgs` type of the `SetLockup` instruction
// which looks like a `Lockup`, but all of the members are wrapped
// with `Option<>`s
typedef enum StakeLockupPresent {
    StakeLockupHasTimestamp = 1 << 0,
    StakeLockupHasEpoch     = 1 << 1,
    StakeLockupHasCustodian = 1 << 2,
    StakeLockupHasAll       = (
        StakeLockupHasTimestamp
        | StakeLockupHasEpoch
        | StakeLockupHasCustodian
    ),
} StakeLockupPresent;

typedef struct StakeLockup {
    StakeLockupPresent present;
    int64_t unix_timestamp;
    uint64_t epoch;
    Pubkey* custodian;
} StakeLockup;

typedef struct StakeInitializeInfo {
    Pubkey* account;
    Pubkey* stake_authority;
    Pubkey* withdraw_authority;
    StakeLockup lockup;
} StakeInitializeInfo;

typedef struct StakeInfo {
    enum StakeInstructionKind kind;
    union {
        StakeDelegateInfo delegate_stake;
        StakeInitializeInfo initialize;
    };
} StakeInfo;

int parse_stake_instructions(Instruction* instruction, MessageHeader* header, StakeInfo* info);
int print_stake_info(StakeInfo* info, MessageHeader* header);
