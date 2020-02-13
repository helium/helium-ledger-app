#include "parser.h"

#define BAIL_IF(x) {int err = x; if (err) return err;}

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

int parse_system_instruction_kind(Parser* parser, enum SystemInstructionKind* kind) {
    return parse_u32(parser, (uint32_t *) kind);
}

int parse_system_transfer(Instruction* instruction, Pubkey* pubkeys, size_t pubkeys_length, Pubkey** from, Pubkey** to, uint64_t* lamports) {
    Parser parser = {instruction->data, instruction->data_length};

    enum SystemInstructionKind kind;
    BAIL_IF(parse_system_instruction_kind(&parser, &kind));
    BAIL_IF(kind != Transfer);
    BAIL_IF(parse_u64(&parser, lamports));

    BAIL_IF(instruction->accounts_length < 2);
    uint8_t from_index = instruction->accounts[0];
    BAIL_IF(from_index >= pubkeys_length);
    *from = &pubkeys[from_index];

    uint8_t to_index = instruction->accounts[1];
    BAIL_IF(to_index >= pubkeys_length);
    *to = &pubkeys[to_index];

    return 0;
}
