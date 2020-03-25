#pragma once

#include "sol/parser.h"

enum ProgramId {
    ProgramIdUnknown = 0,
    ProgramIdStake,
    ProgramIdSystem,
};

enum ProgramId instruction_program_id(const Instruction* instruction, const MessageHeader* header);
int instruction_validate(const Instruction* instruction, const MessageHeader* header);
