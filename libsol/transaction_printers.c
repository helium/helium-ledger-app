#include "instruction.h"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
#include "transaction_printers.h"
#include "util.h"

const InstructionBrief create_stake_account_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    STAKE_IX_BRIEF(StakeInitialize),
};
#define is_create_stake_account(infos, infos_length)\
    instruction_infos_match_briefs(                 \
        infos,                                      \
        create_stake_account_brief,                 \
        infos_length                                \
    )

const InstructionBrief create_stake_account_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    STAKE_IX_BRIEF(StakeInitialize),
};
#define is_create_stake_account_with_seed(infos, infos_length)  \
    instruction_infos_match_briefs(                             \
        infos,                                                  \
        create_stake_account_with_seed_brief,                   \
        infos_length                                            \
    )

const InstructionBrief stake_split_brief_v1_1[] = {
    SYSTEM_IX_BRIEF(SystemAllocate),
    SYSTEM_IX_BRIEF(SystemAssign),
    STAKE_IX_BRIEF(StakeSplit),
};
#define is_stake_split_v1_1(infos, infos_length)\
    instruction_infos_match_briefs(             \
        infos,                                  \
        stake_split_brief_v1_1,                 \
        infos_length                            \
    )

const InstructionBrief stake_split_with_seed_brief_v1_1[] = {
    SYSTEM_IX_BRIEF(SystemAllocateWithSeed),
    STAKE_IX_BRIEF(StakeSplit),
};
#define is_stake_split_with_seed_v1_1(infos, infos_length)  \
    instruction_infos_match_briefs(                         \
        infos,                                              \
        stake_split_with_seed_brief_v1_1,                   \
        infos_length                                        \
    )

const InstructionBrief stake_split_brief_v1_2[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    STAKE_IX_BRIEF(StakeSplit),
};
#define is_stake_split_v1_2(infos, infos_length)\
    instruction_infos_match_briefs(             \
        infos,                                  \
        stake_split_brief_v1_2,                 \
        infos_length                            \
    )

const InstructionBrief stake_split_with_seed_brief_v1_2[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    STAKE_IX_BRIEF(StakeSplit),
};
#define is_stake_split_with_seed_v1_2(infos, infos_length)  \
    instruction_infos_match_briefs(                         \
        infos,                                              \
        stake_split_with_seed_brief_v1_2,                   \
        infos_length                                        \
    )

const InstructionBrief stake_authorize_both_brief[] = {
    STAKE_IX_BRIEF(StakeAuthorize),
    STAKE_IX_BRIEF(StakeAuthorize),
};
#define is_stake_authorize_both(infos, infos_length)\
    instruction_infos_match_briefs(                 \
        infos,                                      \
        stake_authorize_both_brief,                 \
        infos_length                                \
    )

const InstructionBrief create_nonce_account_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    SYSTEM_IX_BRIEF(SystemInitializeNonceAccount),
};
#define is_create_nonce_account(infos, infos_length)    \
    instruction_infos_match_briefs(                     \
        infos,                                          \
        create_nonce_account_brief,                     \
        infos_length                                    \
    )

const InstructionBrief create_nonce_account_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    SYSTEM_IX_BRIEF(SystemInitializeNonceAccount),
};
#define is_create_nonce_account_with_seed(infos, infos_length)  \
    instruction_infos_match_briefs(                             \
        infos,                                                  \
        create_nonce_account_with_seed_brief,                   \
        infos_length                                            \
    )

const InstructionBrief create_vote_account_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    VOTE_IX_BRIEF(VoteInitialize),
};
#define is_create_vote_account(infos, infos_length) \
    instruction_infos_match_briefs(                 \
        infos,                                      \
        create_vote_account_brief,                  \
        infos_length                                \
    )

const InstructionBrief create_vote_account_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    VOTE_IX_BRIEF(VoteInitialize),
};
#define is_create_vote_account_with_seed(infos, infos_length)   \
    instruction_infos_match_briefs(                             \
        infos,                                                  \
        create_vote_account_with_seed_brief,                    \
        infos_length                                            \
    )

const InstructionBrief vote_authorize_both_brief[] = {
    VOTE_IX_BRIEF(VoteAuthorize),
    VOTE_IX_BRIEF(VoteAuthorize),
};
#define is_vote_authorize_both(infos, infos_length) \
    instruction_infos_match_briefs(                 \
        infos,                                      \
        vote_authorize_both_brief,                  \
        infos_length                                \
    )

const InstructionBrief spl_token_create_mint_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    SPL_TOKEN_IX_BRIEF(SplTokenKind(InitializeMint)),
};
#define is_spl_token_create_mint(infos, infos_length)   \
    instruction_infos_match_briefs(                     \
        infos,                                          \
        spl_token_create_mint_brief,                    \
        infos_length                                    \
    )

const InstructionBrief spl_token_create_mint_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    SPL_TOKEN_IX_BRIEF(SplTokenKind(InitializeMint)),
};
#define is_spl_token_create_mint_with_seed(infos, infos_length) \
    instruction_infos_match_briefs(                             \
        infos,                                                  \
        spl_token_create_mint_with_seed_brief,                  \
        infos_length                                            \
    )

const InstructionBrief spl_token_create_account_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    SPL_TOKEN_IX_BRIEF(SplTokenKind(InitializeAccount)),
};
#define is_spl_token_create_account(infos, infos_length)    \
    instruction_infos_match_briefs(                         \
        infos,                                              \
        spl_token_create_account_brief,                     \
        infos_length                                        \
    )

const InstructionBrief spl_token_create_account_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    SPL_TOKEN_IX_BRIEF(SplTokenKind(InitializeAccount)),
};
#define is_spl_token_create_account_with_seed(infos, infos_length)  \
    instruction_infos_match_briefs(                                 \
        infos,                                                      \
        spl_token_create_account_with_seed_brief,                   \
        infos_length                                                \
    )

const InstructionBrief spl_token_create_multisig_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    SPL_TOKEN_IX_BRIEF(SplTokenKind(InitializeMultisig)),
};
#define is_spl_token_create_multisig(infos, infos_length)   \
    instruction_infos_match_briefs(                         \
        infos,                                              \
        spl_token_create_multisig_brief,                    \
        infos_length                                        \
    )

const InstructionBrief spl_token_create_multisig_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    SPL_TOKEN_IX_BRIEF(SplTokenKind(InitializeMultisig)),
};
#define is_spl_token_create_multisig_with_seed(infos, infos_length) \
    instruction_infos_match_briefs(                                 \
        infos,                                                      \
        spl_token_create_multisig_with_seed_brief,                  \
        infos_length                                                \
    )

static int print_create_stake_account(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const StakeInitializeInfo* si_info = &infos[1].stake.initialize;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create stake acct", ca_info->to);

    BAIL_IF(print_system_create_account_info(NULL, ca_info, header));
    BAIL_IF(print_stake_initialize_info(NULL, si_info, header));

    return 0;
}

static int print_create_stake_account_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* cws_info =
        &infos[0].system.create_account_with_seed;
    const StakeInitializeInfo* si_info = &infos[1].stake.initialize;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create stake acct", cws_info->to);

    BAIL_IF(
        print_system_create_account_with_seed_info(NULL, cws_info, header)
    );
    BAIL_IF(print_stake_initialize_info(NULL, si_info, header));

    return 0;
}

static int print_stake_split_with_seed_v1_1(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemAllocateWithSeedInfo* aws_info =
        &infos[0].system.allocate_with_seed;
    const StakeSplitInfo* ss_info = &infos[1].stake.split;

    BAIL_IF(print_stake_split_info1(ss_info, header));
    BAIL_IF(print_system_allocate_with_seed_info(NULL, aws_info, header));
    BAIL_IF(print_stake_split_info2(ss_info, header));

    return 0;
}

static int print_stake_split_with_seed_v1_2(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* cws_info =
        &infos[0].system.create_account_with_seed;
    const StakeSplitInfo* ss_info = &infos[1].stake.split;

    BAIL_IF(print_stake_split_info1(ss_info, header));
    BAIL_IF(print_system_create_account_with_seed_info(NULL, cws_info, header));
    BAIL_IF(print_stake_split_info2(ss_info, header));

    return 0;
}

static int print_stake_authorize_both(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_lenght
) {
    const StakeAuthorizeInfo* staker_info = &infos[0].stake.authorize;
    const StakeAuthorizeInfo* withdrawer_info = &infos[1].stake.authorize;
    SummaryItem* item;

    // Sanity check
    BAIL_IF(staker_info->authorize != StakeAuthorizeStaker);
    BAIL_IF(withdrawer_info->authorize != StakeAuthorizeWithdrawer);

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Set stake auth", staker_info->account);

    if (staker_info->new_authority == withdrawer_info->new_authority) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(
            item,
            "New authorities",
            staker_info->new_authority
        );
    } else {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(
            item,
            "New stake auth",
            staker_info->new_authority
        );

        item = transaction_summary_general_item();
        summary_item_set_pubkey(
            item,
            "New withdraw auth",
            withdrawer_info->new_authority
        );
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Authorized by", withdrawer_info->authority);

    return 0;
}

static int print_create_nonce_account(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const SystemInitializeNonceInfo* ni_info =
        &infos[1].system.initialize_nonce;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create nonce acct", ca_info->to);

    BAIL_IF(print_system_create_account_info(NULL, ca_info, header));
    BAIL_IF(print_system_initialize_nonce_info(NULL, ni_info, header));

    return 0;
}

static int print_create_nonce_account_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* ca_info =
        &infos[0].system.create_account_with_seed;
    const SystemInitializeNonceInfo* ni_info =
        &infos[1].system.initialize_nonce;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create nonce acct", ca_info->to);

    BAIL_IF(print_system_create_account_with_seed_info(NULL, ca_info, header));
    BAIL_IF(print_system_initialize_nonce_info(NULL, ni_info, header));

    return 0;
}

static int print_create_vote_account(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const VoteInitializeInfo* vi_info = &infos[1].vote.initialize;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create vote acct", ca_info->to);

    BAIL_IF(print_system_create_account_info(NULL, ca_info, header));
    BAIL_IF(print_vote_initialize_info(NULL, vi_info, header));

    return 0;
}

static int print_create_vote_account_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* ca_info =
        &infos[0].system.create_account_with_seed;
    const VoteInitializeInfo* vi_info = &infos[1].vote.initialize;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create vote acct", ca_info->to);

    BAIL_IF(print_system_create_account_with_seed_info(NULL, ca_info, header));
    BAIL_IF(print_vote_initialize_info(NULL, vi_info, header));

    return 0;
}

static int print_vote_authorize_both(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_lenght
) {
    const VoteAuthorizeInfo* voter_info = &infos[0].vote.authorize;
    const VoteAuthorizeInfo* withdrawer_info = &infos[1].vote.authorize;
    SummaryItem* item;

    // Sanity check
    BAIL_IF(voter_info->authorize != VoteAuthorizeVoter);
    BAIL_IF(withdrawer_info->authorize != VoteAuthorizeWithdrawer);

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Set vote auth", voter_info->account);

    if (voter_info->new_authority == withdrawer_info->new_authority) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(
            item,
            "New authorities",
            voter_info->new_authority
        );
    } else {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(
            item,
            "New vote auth",
            voter_info->new_authority
        );

        item = transaction_summary_general_item();
        summary_item_set_pubkey(
            item,
            "New withdraw auth",
            withdrawer_info->new_authority
        );
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Authorized by", withdrawer_info->authority);

    return 0;
}

static int print_spl_token_create_mint(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const SplTokenInitializeMintInfo* im_info =
        &infos[1].spl_token.initialize_mint;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create token mint", im_info->mint_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Mint authority", im_info->mint_authority);

    item = transaction_summary_general_item();
    summary_item_set_u64(item, "Mint decimals", im_info->decimals);

    if (im_info->freeze_authority != NULL) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "Freeze authority", im_info->freeze_authority);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Funded by", ca_info->from);

    item = transaction_summary_general_item();
    summary_item_set_amount(item, "Funded with", ca_info->lamports);

    return 0;
}

static int print_spl_token_create_account(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const SplTokenInitializeAccountInfo* ia_info =
        &infos[1].spl_token.initialize_account;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create token acct", ia_info->token_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From mint", ia_info->mint_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Owned by", ia_info->owner);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Funded by", ca_info->from);

    item = transaction_summary_general_item();
    summary_item_set_amount(item, "Funded with", ca_info->lamports);

    return 0;
}

static int print_spl_token_create_multisig(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const SplTokenInitializeMultisigInfo* im_info =
        &infos[1].spl_token.initialize_multisig;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create multisig", im_info->multisig_account);

    item = transaction_summary_general_item();
    summary_item_set_multisig_m_of_n(item, im_info->body.m, im_info->signers.count);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Funded by", ca_info->from);

    item = transaction_summary_general_item();
    summary_item_set_amount(item, "Funded with", ca_info->lamports);

    return 0;
}

static int print_spl_token_create_mint_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* ca_info = &infos[0].system.create_account_with_seed;
    const SplTokenInitializeMintInfo* im_info =
        &infos[1].spl_token.initialize_mint;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create token mint", im_info->mint_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Mint authority", im_info->mint_authority);

    item = transaction_summary_general_item();
    summary_item_set_u64(item, "Mint decimals", im_info->decimals);

    if (im_info->freeze_authority != NULL) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "Freeze authority", im_info->freeze_authority);
    }

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Funded by", ca_info->from);

    item = transaction_summary_general_item();
    summary_item_set_amount(item, "Funded with", ca_info->lamports);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Base", ca_info->base);

    item = transaction_summary_general_item();
    summary_item_set_sized_string(item, "Seed", &ca_info->seed);

    return 0;
}

static int print_spl_token_create_account_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* ca_info = &infos[0].system.create_account_with_seed;
    const SplTokenInitializeAccountInfo* ia_info =
        &infos[1].spl_token.initialize_account;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create token acct", ia_info->token_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "From mint", ia_info->mint_account);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Owned by", ia_info->owner);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Funded by", ca_info->from);

    item = transaction_summary_general_item();
    summary_item_set_amount(item, "Funded with", ca_info->lamports);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Base", ca_info->base);

    item = transaction_summary_general_item();
    summary_item_set_sized_string(item, "Seed", &ca_info->seed);

    return 0;
}

static int print_spl_token_create_multisig_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* ca_info = &infos[0].system.create_account_with_seed;
    const SplTokenInitializeMultisigInfo* im_info =
        &infos[1].spl_token.initialize_multisig;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create multisig", im_info->multisig_account);

    item = transaction_summary_general_item();
    summary_item_set_multisig_m_of_n(item, im_info->body.m, im_info->signers.count);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Funded by", ca_info->from);

    item = transaction_summary_general_item();
    summary_item_set_amount(item, "Funded with", ca_info->lamports);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Base", ca_info->base);

    item = transaction_summary_general_item();
    summary_item_set_sized_string(item, "Seed", &ca_info->seed);

    return 0;
}

int print_transaction(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    if (infos_length > 1) {
        InstructionBrief nonce_brief =
            SYSTEM_IX_BRIEF(SystemAdvanceNonceAccount);
        if (instruction_info_matches_brief(infos, &nonce_brief)) {
            print_system_nonced_transaction_sentinel(&infos->system, header);
            infos++;
            infos_length--;
        }
    }

    switch (infos_length) {
        case 1:
            switch (infos->kind) {
                case ProgramIdSystem:
                    return print_system_info(&infos->system, header);
                case ProgramIdStake:
                    return print_stake_info(&infos->stake, header);
                case ProgramIdVote:
                    return print_vote_info(&infos->vote, header);
                case ProgramIdSplToken:
                    return print_spl_token_info(&infos->spl_token, header);
                case ProgramIdUnknown:
                    break;
            }
            break;
        case 2: {
            if (is_create_stake_account(infos, infos_length)) {
                return print_create_stake_account(header, infos, infos_length);
            } else if (is_create_stake_account_with_seed(infos, infos_length)) {
                return print_create_stake_account_with_seed(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_create_nonce_account(infos, infos_length)) {
                return print_create_nonce_account(header, infos, infos_length);
            } else if (is_create_nonce_account_with_seed(infos, infos_length)) {
                return print_create_nonce_account_with_seed(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_create_vote_account(infos, infos_length)) {
                return print_create_vote_account(header, infos, infos_length);
            } else if (is_create_vote_account_with_seed(infos, infos_length)) {
                return print_create_vote_account_with_seed(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_stake_authorize_both(infos, infos_length)) {
                return print_stake_authorize_both(header, infos, infos_length);
            } else if (is_vote_authorize_both(infos, infos_length)) {
                return print_vote_authorize_both(header, infos, infos_length);
            } else if (is_stake_split_with_seed_v1_1(infos, infos_length)) {
                return print_stake_split_with_seed_v1_1(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_stake_split_v1_2(infos, infos_length)) {
                // System create account is issued with zero lamports in this
                // case, so it has no interesting info to add. Print stake
                // split as if it were a single instruction
                return print_stake_info(&infos[1].stake, header);
            } else if (is_stake_split_with_seed_v1_2(infos, infos_length)) {
                return print_stake_split_with_seed_v1_2(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_spl_token_create_mint(infos, infos_length)) {
                return print_spl_token_create_mint(header, infos, infos_length);
            } else if (is_spl_token_create_account(infos, infos_length)) {
                return print_spl_token_create_account(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_spl_token_create_multisig(infos, infos_length)) {
                return print_spl_token_create_multisig(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_spl_token_create_mint_with_seed(infos, infos_length)) {
                return print_spl_token_create_mint_with_seed(header, infos, infos_length);
            } else if (is_spl_token_create_account_with_seed(infos, infos_length)) {
                return print_spl_token_create_account_with_seed(
                    header,
                    infos,
                    infos_length
                );
            } else if (is_spl_token_create_multisig_with_seed(infos, infos_length)) {
                return print_spl_token_create_multisig_with_seed(
                    header,
                    infos,
                    infos_length
                );
            }
            break;
        }
        case 3: {
            if (is_stake_split_v1_1(infos, infos_length)) {
                // System allocate/assign have no interesting info, print
                // stake split as if it were a single instruction
                return print_stake_info(&infos[2].stake, header);
            }
            break;
        }
        default:
            break;
    }

    return 1;
}
