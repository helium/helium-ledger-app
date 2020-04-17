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

const InstructionBrief stake_split_brief[] = {
    SYSTEM_IX_BRIEF(SystemAllocate),
    SYSTEM_IX_BRIEF(SystemAssign),
    STAKE_IX_BRIEF(StakeSplit),
};
#define is_stake_split(infos, infos_length) \
    instruction_infos_match_briefs(         \
        infos,                              \
        stake_split_brief,                  \
        infos_length                        \
    )

const InstructionBrief stake_split_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemAllocateWithSeed),
    STAKE_IX_BRIEF(StakeSplit),
};
#define is_stake_split_with_seed(infos, infos_length)   \
    instruction_infos_match_briefs(                     \
        infos,                                          \
        stake_split_with_seed_brief,                    \
        infos_length                                    \
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

static int print_stake_split_with_seed(
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
            } else if (is_stake_split_with_seed(infos, infos_length)) {
                return print_stake_split_with_seed(
                    header,
                    infos,
                    infos_length
                );
            }
            break;
        }
        case 3: {
            if (is_stake_split(infos, infos_length)) {
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
