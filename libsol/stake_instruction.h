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
    const Pubkey* stake_pubkey;
    const Pubkey* vote_pubkey;
    const Pubkey* authorized_pubkey;
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
    const Pubkey* custodian;
} StakeLockup;

typedef struct StakeInitializeInfo {
    const Pubkey* account;
    const Pubkey* stake_authority;
    const Pubkey* withdraw_authority;
    StakeLockup lockup;
} StakeInitializeInfo;

typedef struct StakeInfo {
    enum StakeInstructionKind kind;
    union {
        StakeDelegateInfo delegate_stake;
        StakeInitializeInfo initialize;
    };
} StakeInfo;

int parse_stake_instructions(const Instruction* instruction, const MessageHeader* header, StakeInfo* info);
int print_stake_info(const StakeInfo* info, const MessageHeader* header);

int print_stake_initialize_info(
    const char* primary_title,
    const StakeInitializeInfo* info,
    const MessageHeader* header
);
