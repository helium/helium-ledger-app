#include "common_byte_strings.h"
#include "instruction.h"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
#include "spl_token_instruction.h"
#include "util.h"

const Pubkey spl_token_program_id = {{
    PROGRAM_ID_SPL_TOKEN
}};

static int parse_spl_token_instruction_kind(
    Parser* parser,
    SplTokenInstructionKind* kind
) {
    uint8_t maybe_kind;
    BAIL_IF(parse_u8(parser, &maybe_kind));
    switch (maybe_kind) {
        case SplTokenKind(InitializeMint):
        case SplTokenKind(InitializeAccount):
        case SplTokenKind(InitializeMultisig):
        case SplTokenKind(Transfer):
        case SplTokenKind(Approve):
        case SplTokenKind(Revoke):
        case SplTokenKind(SetOwner):
        case SplTokenKind(MintTo):
        case SplTokenKind(Burn):
        case SplTokenKind(CloseAccount):
            *kind = (SplTokenInstructionKind) maybe_kind;
            return 0;
    }
    return 1;
}

static int parse_initialize_mint_spl_token_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenInitializeMintInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));
    BAIL_IF(parse_u8(parser, &info->body.decimals));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    if (info->body.amount == 0) {
        info->token_account = NULL;
        BAIL_IF(instruction_accounts_iterator_next(&it, &info->owner));
    } else {
        BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
        BAIL_IF(instruction_accounts_iterator_next(&it, &info->owner));
    }
    return 0;
}

static int parse_initialize_account_spl_token_instruction(
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenInitializeAccountInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->owner));

    return 0;
}

static int parse_initialize_multisig_spl_token_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenInitializeMultisigInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u8(parser, &info->body.m));
    BAIL_IF(info->body.m > Token_MAX_SIGNERS);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->multisig_account));

    size_t n = instruction_accounts_iterator_remaining(&it);
    BAIL_IF(n > Token_MAX_SIGNERS);
    for (size_t i = 0; i < n; i++) {
        BAIL_IF(instruction_accounts_iterator_next(&it, &info->signers[i]));
    }
    info->signers[n] = NULL;

    return 0;
}

static int parse_spl_token_sign(
    InstructionAccountsIterator* it,
    SplTokenSign* sign
) {
    size_t n = instruction_accounts_iterator_remaining(it);
    BAIL_IF(n == 0);

    if (n == 1) {
        sign->kind = SplTokenSignKindSingle;
        BAIL_IF(instruction_accounts_iterator_next(it, &sign->single.signer));
    } else {
        sign->kind = SplTokenSignKindMulti;
        BAIL_IF(instruction_accounts_iterator_next(it, &sign->multi.account));
        for (size_t i = 0; i < n; i++) {
            BAIL_IF(instruction_accounts_iterator_next(it, &sign->multi.signers[i]));
        }
        sign->multi.signers[n] = NULL;
    }
    return 0;
}

static int parse_transfer_spl_token_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenTransferInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->src_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->dest_account));

    BAIL_IF(parse_spl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_approve_spl_token_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenApproveInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->delegate));

    BAIL_IF(parse_spl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_revoke_spl_token_instruction(
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenRevokeInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));

    BAIL_IF(parse_spl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_set_owner_spl_token_instruction(
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenSetOwnerInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->new_owner));

    BAIL_IF(parse_spl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_mint_to_spl_token_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenMintToInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->mint_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));

    BAIL_IF(parse_spl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_burn_spl_token_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenBurnInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(parse_u64(parser, &info->body.amount));

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));

    BAIL_IF(parse_spl_token_sign(&it, &info->sign));

    return 0;
}

static int parse_close_account_spl_token_instruction(
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenCloseAccountInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->token_account));
    BAIL_IF(instruction_accounts_iterator_next(&it, &info->dest_account));

    BAIL_IF(parse_spl_token_sign(&it, &info->sign));

    return 0;
}

int parse_spl_token_instructions(
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenInfo* info
) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_spl_token_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case SplTokenKind(InitializeMint):
            return parse_initialize_mint_spl_token_instruction(
                &parser,
                instruction,
                header,
                &info->initialize_mint
            );
        case SplTokenKind(InitializeAccount):
            return parse_initialize_account_spl_token_instruction(
                instruction,
                header,
                &info->initialize_account
            );
        case SplTokenKind(InitializeMultisig):
            return parse_initialize_multisig_spl_token_instruction(
                &parser,
                instruction,
                header,
                &info->initialize_multisig
            );
        case SplTokenKind(Transfer):
            return parse_transfer_spl_token_instruction(
                &parser,
                instruction,
                header,
                &info->transfer
            );
        case SplTokenKind(Approve):
            return parse_approve_spl_token_instruction(
                &parser,
                instruction,
                header,
                &info->approve
            );
        case SplTokenKind(Revoke):
            return parse_revoke_spl_token_instruction(
                instruction,
                header,
                &info->revoke
            );
        case SplTokenKind(SetOwner):
            return parse_set_owner_spl_token_instruction(
                instruction,
                header,
                &info->set_owner
            );
        case SplTokenKind(MintTo):
            return parse_mint_to_spl_token_instruction(
                &parser,
                instruction,
                header,
                &info->mint_to
            );
        case SplTokenKind(Burn):
            return parse_burn_spl_token_instruction(
                &parser,
                instruction,
                header,
                &info->burn
            );
        case SplTokenKind(CloseAccount):
            return parse_close_account_spl_token_instruction(
                instruction,
                header,
                &info->close_account
            );
    }
    return 1;
}

int print_spl_token_info(
    const SplTokenInfo* info,
    const MessageHeader* header
) {
    return 1;
}
