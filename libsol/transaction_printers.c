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
        info,                                       \
        create_stake_account_brief,                 \
        infos_length                                \
    )

const InstructionBrief create_stake_account_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    STAKE_IX_BRIEF(StakeInitialize),
};
#define is_create_stake_account_with_seed(infos, infos_length)  \
    instruction_infos_match_briefs(                             \
        info,                                                   \
        create_stake_account_with_seed_brief,                   \
        infos_length                                            \
    )

const InstructionBrief create_nonce_account[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccount),
    SYSTEM_IX_BRIEF(SystemInitializeNonceAccount),
};
#define is_create_nonce_account(infos, infos_length)   \
    instruction_infos_match_briefs(                         \
        info,                                               \
        create_nonce_account,                               \
        infos_length                                        \
    )

const InstructionBrief create_nonce_account_with_seed_brief[] = {
    SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
    SYSTEM_IX_BRIEF(SystemInitializeNonceAccount),
};
#define is_create_nonce_account_with_seed(infos, infos_length)  \
    instruction_infos_match_briefs(                             \
        info,                                                   \
        create_nonce_account_with_seed_brief,                   \
        infos_length                                            \
    )

static int print_create_stake_account(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const StakeInitializeInfo* si_info = &infos[1].stake.initialize;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create stake Acct", ca_info->to);

    BAIL_IF(print_system_create_account_info(NULL, ca_info, header));
    BAIL_IF(print_stake_initialize_info(NULL, si_info, header));

    return 0;
}

static int print_create_stake_account_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* cws_info = &infos[0].system.create_account_with_seed;
    const StakeInitializeInfo* si_info = &infos[1].stake.initialize;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create stake Acct", cws_info->to);

    BAIL_IF(print_system_create_account_with_seed_info(NULL, cws_info, header));
    BAIL_IF(print_stake_initialize_info(NULL, si_info, header));

    return 0;
}

static int print_create_nonce_account(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountInfo* ca_info = &infos[0].system.create_account;
    const SystemInitializeNonceInfo* ni_info = &infos[1].system.initialize_nonce;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create nonce Acct", ca_info->to);

    BAIL_IF(print_system_create_account_info(NULL, ca_info, header));
    BAIL_IF(print_system_initialize_nonce_info(NULL, ni_info, header));

    return 0;
}

static int print_create_nonce_account_with_seed(
    const MessageHeader* header,
    const InstructionInfo* infos,
    size_t infos_length
) {
    const SystemCreateAccountWithSeedInfo* ca_info = &infos[0].system.create_account_with_seed;
    const SystemInitializeNonceInfo* ni_info = &infos[1].system.initialize_nonce;

    SummaryItem* item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Create nonce Acct", ca_info->to);

    BAIL_IF(print_system_create_account_with_seed_info(NULL, ca_info, header));
    BAIL_IF(print_system_initialize_nonce_info(NULL, ni_info, header));

    return 0;
}

int print_transaction(const MessageHeader* header, const InstructionInfo* infos, size_t infos_length) {
    size_t operative_ix = 0;
    const InstructionInfo* info = &infos[operative_ix];
    if (infos_length > 1) {
        InstructionBrief nonce_brief = SYSTEM_IX_BRIEF(SystemAdvanceNonceAccount);
        if (instruction_info_matches_brief(info, &nonce_brief)) {
            print_system_nonced_transaction_sentinel(&info->system, header);
            operative_ix++;
            infos_length--;
            info = &infos[operative_ix];
        }
    }

    switch (infos_length) {
        case 1:
            switch (info->kind) {
                case ProgramIdSystem:
                    return print_system_info(&info->system, header);
                case ProgramIdStake:
                    return print_stake_info(&info->stake, header);
                case ProgramIdUnknown:
                    break;
            }
            break;
        case 2: {
            if (is_create_stake_account(infos, infos_length)) {
                return print_create_stake_account(header, info, infos_length);
            } else if (is_create_stake_account_with_seed(infos, infos_length)) {
                return print_create_stake_account_with_seed(header, info, infos_length);
            } else if (is_create_nonce_account(infos, infos_length)) {
                return print_create_nonce_account(header, info, infos_length);
            } else if (is_create_nonce_account_with_seed(infos, infos_length)) {
                return print_create_nonce_account_with_seed(header, info, infos_length);
            }
        }
        default:
            break;
    }

    return 1;
}
