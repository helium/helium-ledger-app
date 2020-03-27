#pragma once

#include "sol/parser.h"
#include "stake_instruction.h"
#include "system_instruction.h"

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
