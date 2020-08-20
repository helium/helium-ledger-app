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

static int print_spl_token_sign(const SplTokenSign* sign) {
    SummaryItem* item;

    const Pubkey* owner;
    if (sign->kind == SplTokenSignKindSingle) {
        owner = sign->single.signer;
    } else {
        owner = sign->multi.account;
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Owner", owner);

    return 0;
}

static int print_spl_token_initialize_mint_info(
    const char* primary_title,
    const SplTokenInitializeMintInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->mint_account);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Mint owner", info->owner);

    item = transaction_summary_general_item();
    summary_item_set_u64(item, "Decimals", info->body.decimals);

    if (info->body.amount != 0) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "Token account", info->token_account);

        item = transaction_summary_general_item();
        summary_item_set_token_amount(item, "Token balance", info->body.amount, NULL, info->body.decimals);
    }

    return 0;
}

static int print_spl_token_initialize_account_info(
    const char* primary_title,
    const SplTokenInitializeAccountInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->token_account);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Token account owner", info->owner);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Token mint", info->mint_account);

    return 0;
}

static char multisig_m_of_n_string[9]; // worst case "11 of 11"
static int set_multisig_m_of_n_string(uint8_t m, uint8_t n) {
    BAIL_IF(m > Token_MAX_SIGNERS);
    BAIL_IF(n > Token_MAX_SIGNERS);

    size_t i = 0;
    if (m > 9) multisig_m_of_n_string[i++] = '1';
    multisig_m_of_n_string[i++] = '0' + (m % 10);
    strncpy(&multisig_m_of_n_string[i], " of ", 5);
    i += 4;
    if (n > 9) multisig_m_of_n_string[i++] = '1';
    multisig_m_of_n_string[i++] = '0' + (n % 10);
    multisig_m_of_n_string[i++] = '\0';

    return 0;
}

static uint8_t spl_token_multisigners_length(const SplTokenMultiSigners* multi_signers) {
    uint8_t length = 0;
    for (; length <= Token_MAX_SIGNERS && multi_signers[length]; length++);
    return length;
}

static int print_spl_token_initialize_multisig_info(
    const char* primary_title,
    const SplTokenInitializeMultisigInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->multisig_account);
    }

    uint8_t n = spl_token_multisigners_length(&info->signers);
    set_multisig_m_of_n_string(info->body.m, n);
    item = transaction_summary_general_item();
    summary_item_set_string(item, "Required Signers", multisig_m_of_n_string);

    return 0;
}

static int print_spl_token_transfer_info(
    const SplTokenTransferInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_u64(item, "SPL Token transfer", info->body.amount);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From", info->src_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "To", info->dest_account);

    print_spl_token_sign(&info->sign);

    return 0;
}

static int print_spl_token_approve_info(
    const SplTokenApproveInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Approve delegate", info->delegate);

    item = transaction_summary_general_item();
    summary_item_set_u64(item, "To spend", info->body.amount);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From", info->token_account);

    print_spl_token_sign(&info->sign);

    return 0;
}

static int print_spl_token_revoke_info(
    const SplTokenRevokeInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Revoke delegate", info->token_account);

    print_spl_token_sign(&info->sign);

    return 0;
}

static int print_spl_token_set_owner_info(
    const SplTokenSetOwnerInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Set account owner", info->token_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "New owner", info->new_owner);

    print_spl_token_sign(&info->sign);

    return 0;
}

static int print_spl_token_mint_to_info(
    const SplTokenMintToInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_u64(item, "Mint tokens", info->body.amount);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From mint", info->mint_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "To account", info->token_account);

    print_spl_token_sign(&info->sign);

    return 0;
}

static int print_spl_token_burn_info(
    const SplTokenBurnInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_u64(item, "Burn tokens", info->body.amount);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From account", info->token_account);

    print_spl_token_sign(&info->sign);

    return 0;
}

static int print_spl_token_close_account_info(
    const SplTokenCloseAccountInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Close token acct", info->token_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Withdraw to", info->dest_account);

    print_spl_token_sign(&info->sign);

    return 0;
}

int print_spl_token_info(
    const SplTokenInfo* info,
    const MessageHeader* header
) {
    switch (info->kind) {
        case SplTokenKind(InitializeMint):
            return print_spl_token_initialize_mint_info(
                "Init SPL Token mint",
                &info->initialize_mint,
                header
            );
        case SplTokenKind(InitializeAccount):
            return print_spl_token_initialize_account_info(
                "Init SPL Token acct",
                &info->initialize_account,
                header
            );
        case SplTokenKind(InitializeMultisig):
            return print_spl_token_initialize_multisig_info(
                "Init SPL Token multisig",
                &info->initialize_multisig,
                header
            );
        case SplTokenKind(Transfer):
            return print_spl_token_transfer_info(&info->transfer, header);
        case SplTokenKind(Approve):
            return print_spl_token_approve_info(&info->approve, header);
        case SplTokenKind(Revoke):
            return print_spl_token_revoke_info(&info->revoke, header);
        case SplTokenKind(SetOwner):
            return print_spl_token_set_owner_info(&info->set_owner, header);
        case SplTokenKind(MintTo):
            return print_spl_token_mint_to_info(&info->mint_to, header);
        case SplTokenKind(Burn):
            return print_spl_token_burn_info(&info->burn, header);
        case SplTokenKind(CloseAccount):
            return print_spl_token_close_account_info(&info->close_account, header);
    }

    return 1;
}
