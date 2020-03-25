#pragma once

#include "sol/parser.h"

extern const Pubkey system_program_id;

enum SystemInstructionKind {
    CreateAccount,
    Assign,
    Transfer,
    CreateAccountWithSeed,
    AdvanceNonceAccount,
    WithdrawNonceAccount,
    InitializeNonceAccount,
    AuthorizeNonceAccount,
    Allocate,
    AllocateWithSeed,
    AssignWithSeed
};

typedef struct SystemTransferInfo {
    Pubkey* from;
    Pubkey* to;
    uint64_t lamports;
} SystemTransferInfo;

typedef struct SystemInfo {
    enum SystemInstructionKind kind;
    union {
        SystemTransferInfo transfer;
    };
} SystemInfo;

int parse_system_instructions(Instruction* instruction, MessageHeader* header, SystemInfo* info);
int print_system_info(SystemInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used);
