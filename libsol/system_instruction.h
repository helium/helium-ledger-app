#pragma once

#include "sol/parser.h"

extern const Pubkey system_program_id;

enum SystemInstructionKind {
    SystemCreateAccount,
    SystemAssign,
    SystemTransfer,
    SystemCreateAccountWithSeed,
    SystemAdvanceNonceAccount,
    SystemWithdrawNonceAccount,
    SystemInitializeNonceAccount,
    SystemAuthorizeNonceAccount,
    SystemAllocate,
    SystemAllocateWithSeed,
    SystemAssignWithSeed
};

typedef struct SystemCreateAccountWithSeedInfo {
    const Pubkey* from;
    const Pubkey* to;
    const Pubkey* base;
    SizedString seed;
    uint64_t lamports;
} SystemCreateAccountWithSeedInfo;

typedef struct SystemTransferInfo {
    const Pubkey* from;
    const Pubkey* to;
    uint64_t lamports;
} SystemTransferInfo;

typedef struct SystemAdvanceNonceInfo {
    const Pubkey* account;
    const Pubkey* authority;
} SystemAdvanceNonceInfo;

typedef struct SystemInfo {
    enum SystemInstructionKind kind;
    union {
        SystemTransferInfo transfer;
        SystemCreateAccountWithSeedInfo create_account_with_seed;
        SystemAdvanceNonceInfo advance_nonce;
    };
} SystemInfo;

int parse_system_instructions(const Instruction* instruction, const MessageHeader* header, SystemInfo* info);
int print_system_info(const SystemInfo* info, const MessageHeader* header);
int print_system_nonced_transaction_sentinel(const SystemInfo* info, const MessageHeader* header);
