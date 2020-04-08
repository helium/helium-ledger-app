#pragma once

#include "sol/parser.h"
#include "stake_instruction.h"
#include "system_instruction.h"
#include <stdbool.h>

enum ProgramId {
    ProgramIdUnknown = 0,
    ProgramIdStake,
    ProgramIdSystem,
};

typedef struct InstructionInfo {
    enum ProgramId kind;
    union {
        StakeInfo stake;
        SystemInfo system;
    };
} InstructionInfo;

enum ProgramId instruction_program_id(const Instruction* instruction, const MessageHeader* header);
int instruction_validate(const Instruction* instruction, const MessageHeader* header);


typedef struct InstructionBrief {
    enum ProgramId program_id;
    union {
        enum SystemInstructionKind system;
        enum StakeInstructionKind stake;
    };
} InstructionBrief;

#define SYSTEM_IX_BRIEF(system_ix) { ProgramIdSystem, .system = (system_ix) }
#define STAKE_IX_BRIEF(stake_ix) { ProgramIdStake, .stake = (stake_ix) }

bool instruction_info_matches_brief(const InstructionInfo* info, const InstructionBrief* brief);
bool instruction_infos_match_briefs(const InstructionInfo* infos, const InstructionBrief* briefs, size_t len);
