#include "instruction.h"
#include "stake_instruction.h"
#include "system_instruction.h"
#include "util.h"
#include <string.h>

enum ProgramId instruction_program_id(const Instruction* instruction, const MessageHeader* header) {
    const Pubkey* program_id = &header->pubkeys[instruction->program_id_index];
    if (memcmp(program_id, &system_program_id, PUBKEY_SIZE) == 0) {
        return ProgramIdSystem;
    } else if (memcmp(program_id, &stake_program_id, PUBKEY_SIZE) == 0) {
        return ProgramIdStake;
    }

    return ProgramIdUnknown;
}

int instruction_validate(const Instruction* instruction, const MessageHeader* header) {
    BAIL_IF(instruction->program_id_index >= header->pubkeys_header.pubkeys_length);
    for (size_t i = 0; i < instruction->accounts_length; i++) {
        BAIL_IF(instruction->accounts[i] >= header->pubkeys_header.pubkeys_length);
    }
    return 0;
}

bool instruction_info_matches_brief(const InstructionInfo* info, const InstructionBrief* brief) {
    if (brief->program_id == info->kind) {
        switch (brief->program_id) {
            case ProgramIdStake: return (brief->stake == info->stake.kind);
            case ProgramIdSystem: return (brief->system == info->system.kind);
            case ProgramIdUnknown: break;
        }
    }
    return false;
}
