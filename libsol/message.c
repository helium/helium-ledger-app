#include "instruction.h"
#include "sol/parser.h"
#include "sol/message.h"
#include "sol/transaction_summary.h"
#include "system_instruction.h"
#include "stake_instruction.h"
#include "util.h"
#include <string.h>

#define MAX_INSTRUCTIONS 3

int process_message_body(const uint8_t* message_body, int message_body_length, const MessageHeader* header) {
    BAIL_IF(header->instructions_length == 0);
    BAIL_IF(header->instructions_length > MAX_INSTRUCTIONS);

    InstructionInfo instruction_info[MAX_INSTRUCTIONS];
    memset(instruction_info, 0, sizeof(InstructionInfo) * MAX_INSTRUCTIONS);

    Parser parser = {message_body, message_body_length};
    size_t instruction_count = 0;
    for (; instruction_count < header->instructions_length; instruction_count++) {
        Instruction instruction;
        BAIL_IF(parse_instruction(&parser, &instruction));
        BAIL_IF(instruction_validate(&instruction, header));

        InstructionInfo* info = &instruction_info[instruction_count];
        enum ProgramId program_id = instruction_program_id(&instruction, header);
        switch (program_id) {
            case ProgramIdSystem:
            {
                if (parse_system_instructions(&instruction, header, &info->system) == 0) {
                    info->kind = program_id;
                }
                break;
            }
            case ProgramIdStake:
            {
                if (parse_stake_instructions(&instruction, header, &info->stake) == 0) {
                    info->kind = program_id;
                }
                break;
            }
            case ProgramIdUnknown:
                break;
        }
    }

    // Ensure we've consumed the entire message body
    BAIL_IF(!parser_is_empty(&parser));

    // If we don't know about all of the instructions, bail
    for (size_t i = 0; i < instruction_count; i++) {
        BAIL_IF(instruction_info[i].kind == ProgramIdUnknown);
    }

    size_t operative_ix = 0;
    InstructionInfo* info = &instruction_info[operative_ix];
    if (instruction_count > 1) {
        InstructionBrief nonce_brief = SYSTEM_IX_BRIEF(SystemAdvanceNonceAccount);
        if (instruction_info_matches_brief(info, &nonce_brief)) {
            print_system_nonced_transaction_sentinel(&info->system, header);
            operative_ix++;
            instruction_count--;
            info = &instruction_info[operative_ix];
        }
    }

    switch (instruction_count) {
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
            const InstructionBrief system_create_stake_account_with_seed_brief[] = {
                SYSTEM_IX_BRIEF(SystemCreateAccountWithSeed),
                STAKE_IX_BRIEF(StakeInitialize),
            };
            if (instruction_infos_match_briefs(info, system_create_stake_account_with_seed_brief, 2)) {
                SystemCreateAccountWithSeedInfo* cws_info = &info[0].system.create_account_with_seed;
                StakeInitializeInfo* si_info = &info[1].stake.initialize;

                SummaryItem* item;
                item = transaction_summary_primary_item();
                summary_item_set_pubkey(item, "New stake account", cws_info->to);

                item = transaction_summary_general_item();
                summary_item_set_amount(item, "Transfer", cws_info->lamports);

                item = transaction_summary_general_item();
                summary_item_set_pubkey(item, "From", cws_info->from);

                item = transaction_summary_general_item();
                summary_item_set_pubkey(item, "Base", cws_info->base);

                item = transaction_summary_general_item();
                summary_item_set_sized_string(item, "Seed", &cws_info->seed);

                item = transaction_summary_general_item();
                summary_item_set_pubkey(item, "New stake auth", si_info->stake_authority);

                item = transaction_summary_general_item();
                summary_item_set_pubkey(item, "New withdraw auth", si_info->withdraw_authority);

                item = transaction_summary_general_item();
                summary_item_set_i64(item, "Lockup time", si_info->lockup.unix_timestamp);

                item = transaction_summary_general_item();
                summary_item_set_u64(item, "Lockup epoch", si_info->lockup.epoch);

                item = transaction_summary_general_item();
                summary_item_set_pubkey(item, "Lockup custodian", si_info->lockup.custodian);

                return 0;
            }
        }
        default:
            break;
    }

    return 1;
}
