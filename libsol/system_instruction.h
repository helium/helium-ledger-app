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

typedef struct SystemTransferInfo {
    Pubkey* from;
    Pubkey* to;
    uint64_t lamports;
} SystemTransferInfo;

typedef struct SystemAdvanceNonceInfo {
    Pubkey* account;
    Pubkey* authority;
} SystemAdvanceNonceInfo;

typedef struct SystemInfo {
    enum SystemInstructionKind kind;
    union {
        SystemTransferInfo transfer;
        SystemAdvanceNonceInfo advance_nonce;
    };
} SystemInfo;

int parse_system_instructions(Instruction* instruction, MessageHeader* header, SystemInfo* info);
int print_system_info(SystemInfo* info, MessageHeader* header);
int print_system_nonced_transaction_sentinel(SystemInfo* info, MessageHeader* header);
