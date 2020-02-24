#pragma once

#include "sol/parser.h"

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

int parse_system_transfer_instructions(Parser* parser, MessageHeader* header, SystemTransferInfo* info);

int print_system_transfer_info(SystemTransferInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used);
