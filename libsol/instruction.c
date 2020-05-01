#include "instruction.h"
#include "stake_instruction.h"
#include "system_instruction.h"
#include "util.h"
#include <string.h>

enum ProgramId instruction_program_id(
    const Instruction* instruction,
    const MessageHeader* header
) {
    const Pubkey* program_id = &header->pubkeys[instruction->program_id_index];
    if (memcmp(program_id, &system_program_id, PUBKEY_SIZE) == 0) {
        return ProgramIdSystem;
    } else if (memcmp(program_id, &stake_program_id, PUBKEY_SIZE) == 0) {
        return ProgramIdStake;
    } else if (memcmp(program_id, &vote_program_id, PUBKEY_SIZE) == 0) {
        return ProgramIdVote;
    }

    return ProgramIdUnknown;
}

int instruction_validate(
    const Instruction* instruction,
    const MessageHeader* header
) {
    BAIL_IF(
        instruction->program_id_index >= header->pubkeys_header.pubkeys_length
    );
    for (size_t i = 0; i < instruction->accounts_length; i++) {
        BAIL_IF(
            instruction->accounts[i] >= header->pubkeys_header.pubkeys_length
        );
    }
    return 0;
}

bool instruction_info_matches_brief(
    const InstructionInfo* info,
    const InstructionBrief* brief
) {
    if (brief->program_id == info->kind) {
        switch (brief->program_id) {
            case ProgramIdStake: return (brief->stake == info->stake.kind);
            case ProgramIdSystem: return (brief->system == info->system.kind);
            case ProgramIdVote: return (brief->vote == info->vote.kind);
            case ProgramIdUnknown: break;
        }
    }
    return false;
}

bool instruction_infos_match_briefs(
    const InstructionInfo* infos,
    const InstructionBrief* briefs,
    size_t len
) {
    size_t i;
    for (i = 0; i < len; i++) {
        if (!instruction_info_matches_brief(&infos[i], &briefs[i])) {
            break;
        }
    }
    return (i == len);
}

void instruction_accounts_iterator_init(
    InstructionAccountsIterator* it,
    const MessageHeader* header,
    const Instruction* instruction
) {
    it->message_header_pubkeys = header->pubkeys;
    it->instruction_accounts_length = instruction->accounts_length;
    it->instruction_accounts = instruction->accounts;
    it->current_instruction_account = 0;
}

int instruction_accounts_iterator_next(
    InstructionAccountsIterator* it,
    const Pubkey** next_account
) {
    if (it->current_instruction_account < it->instruction_accounts_length) {
        size_t pubkeys_index =
            it->instruction_accounts[it->current_instruction_account++];
        if (next_account) {
            *next_account = &it->message_header_pubkeys[pubkeys_index];
        }
        return 0;
    }
    return 1;
}
