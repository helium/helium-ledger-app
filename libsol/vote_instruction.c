#include "common_byte_strings.h"
#include "sol/transaction_summary.h"
#include "util.h"
#include "vote_instruction.h"

const Pubkey vote_program_id = {{ PROGRAM_ID_VOTE }};

static int parse_vote_instruction_kind(
    Parser* parser,
    enum VoteInstructionKind* kind
) {
    uint32_t maybe_kind;
    BAIL_IF(parse_u32(parser, &maybe_kind));
    switch (maybe_kind) {
        case VoteInitialize:
        case VoteAuthorize:
        case VoteVote:
        case VoteWithdraw:
        case VoteUpdateNode:
            *kind = (enum VoteInstructionKind) maybe_kind;
            return 0;
    }
    return 1;
}

static int parse_vote_authorize(
    Parser* parser,
    enum VoteAuthorize* authorize
) {
    uint32_t maybe_authorize;
    BAIL_IF(parse_u32(parser, &maybe_authorize));
    switch (maybe_authorize) {
        case VoteAuthorizeVoter:
        case VoteAuthorizeWithdrawer:
            *authorize = (enum VoteAuthorize) maybe_authorize;
            return 0;
    }
    return 1;
}

static int parse_vote_initialize_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    VoteInitializeInfo* info
) {
    BAIL_IF(instruction->accounts_length < 3);
    size_t accounts_index = 0;
    size_t pubkeys_index = instruction->accounts[accounts_index++];
    info->account = &header->pubkeys[pubkeys_index];

    accounts_index++; // Skip rent sysvar
    accounts_index++; // Skip clock sysvar

    BAIL_IF(parse_pubkey(parser, &info->vote_init.node));
    BAIL_IF(parse_pubkey(parser, &info->vote_init.vote_authority));
    BAIL_IF(parse_pubkey(parser, &info->vote_init.withdraw_authority));
    uint8_t commission;
    BAIL_IF(parse_u8(parser, &commission));
    info->vote_init.commission = commission;

    return 0;
}

static int parse_vote_withdraw_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    VoteWithdrawInfo* info
) {
    BAIL_IF(instruction->accounts_length < 3);
    size_t accounts_index = 0;
    size_t pubkeys_index = instruction->accounts[accounts_index++];
    info->account = &header->pubkeys[pubkeys_index];

    pubkeys_index = instruction->accounts[accounts_index++];
    info->to = &header->pubkeys[pubkeys_index];

    pubkeys_index = instruction->accounts[accounts_index++];
    info->authority = &header->pubkeys[pubkeys_index];

    BAIL_IF(parse_u64(parser, &info->lamports));

    return 0;
}

static int parse_vote_authorize_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    VoteAuthorizeInfo* info
) {
    BAIL_IF(instruction->accounts_length < 3);
    size_t accounts_index = 0;
    size_t pubkeys_index = instruction->accounts[accounts_index++];
    info->account = &header->pubkeys[pubkeys_index];

    accounts_index++; // Skip clock sysvar

    pubkeys_index = instruction->accounts[accounts_index++];
    info->authority = &header->pubkeys[pubkeys_index];

    BAIL_IF(parse_pubkey(parser, &info->new_authority));
    BAIL_IF(parse_vote_authorize(parser, &info->authorize));

    return 0;
}

static int parse_vote_update_node_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    VoteUpdateNodeInfo* info
) {
    BAIL_IF(instruction->accounts_length < 3);
    size_t accounts_index = 0;
    size_t pubkeys_index = instruction->accounts[accounts_index++];
    info->account = &header->pubkeys[pubkeys_index];

    if (instruction->data_length == sizeof(uint32_t)) {
        // 1.0.8+, 1.1.3+ format
        // https://github.com/solana-labs/solana/pull/8947
        pubkeys_index = instruction->accounts[accounts_index++];
        info->node_identity = &header->pubkeys[pubkeys_index];
    } else if (
        instruction->data_length == (sizeof(uint32_t) + sizeof(Pubkey))
    ) {
        // Before 1.0.8 and 1.1.3, the node identity was passed as an
        // instruction arg
        BAIL_IF(parse_pubkey(parser, &info->node_identity));
        accounts_index++; // Skip clock sysvar
    } else {
        return 1;
    }

    pubkeys_index = instruction->accounts[accounts_index++];
    info->authority = &header->pubkeys[pubkeys_index];

    return 0;
}

int parse_vote_instructions(
    const Instruction* instruction,
    const MessageHeader* header,
    VoteInfo* info
) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_vote_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case VoteInitialize:
            return parse_vote_initialize_instruction(
                &parser,
                instruction,
                header,
                &info->initialize
            );
        case VoteWithdraw:
            return parse_vote_withdraw_instruction(
                &parser,
                instruction,
                header,
                &info->withdraw
            );
        case VoteAuthorize:
            return parse_vote_authorize_instruction(
                &parser,
                instruction,
                header,
                &info->authorize
            );
        case VoteUpdateNode:
            return parse_vote_update_node_instruction(
                &parser,
                instruction,
                header,
                &info->update_node
            );
        case VoteVote:
            break;
    }

    return 1;
}

static int print_vote_withdraw_info(
    const VoteWithdrawInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_amount(item, "Vote withdraw", info->lamports);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From", info->account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "To", info->to);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Authorized by", info->authority);

    return 0;
}

static int print_vote_authorize_info(
    const VoteAuthorizeInfo* info,
    const MessageHeader* header
) {
    const char* new_authority_title = NULL;
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Set vote auth.", info->account);

    switch (info->authorize) {
        case VoteAuthorizeVoter:
            new_authority_title = "New vote auth.";
            break;
        case VoteAuthorizeWithdrawer:
            new_authority_title = "New w/d auth.";
            break;
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, new_authority_title, info->new_authority);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Authorized by", info->authority);

    return 0;
}

static int print_vote_update_node_info(
    const VoteUpdateNodeInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Update vote node", info->account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "New node ID", info->node_identity);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Authorized by", info->authority);

    return 0;
}

int print_vote_info(const VoteInfo* info, const MessageHeader* header) {
    switch (info->kind) {
        case VoteInitialize:
            return print_vote_initialize_info(
                "Init. vote acct.",
                &info->initialize,
                header
            );
        case VoteWithdraw:
            return print_vote_withdraw_info(
                &info->withdraw,
                header
            );
        case VoteAuthorize:
            return print_vote_authorize_info(
                &info->authorize,
                header
            );
        case VoteUpdateNode:
            return print_vote_update_node_info(
                &info->update_node,
                header
            );
        case VoteVote:
            break;
    }

    return 1;
}

int print_vote_initialize_info(
    const char* primary_title,
    const VoteInitializeInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;
    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->account);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Node", info->vote_init.node);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(
        item,
        "New vote auth.",
        info->vote_init.vote_authority
    );

    item = transaction_summary_general_item();
    summary_item_set_pubkey(
        item,
        "New withdraw auth.",
        info->vote_init.withdraw_authority
    );

    item = transaction_summary_general_item();
    summary_item_set_u64(item, "Commission", info->vote_init.commission);

    return 0;
}
