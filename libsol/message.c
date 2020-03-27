#include "instruction.h"
#include "sol/parser.h"
#include "sol/printer.h"
#include "sol/message.h"
#include "system_instruction.h"
#include "stake_instruction.h"
#include "util.h"
#include <string.h>

int process_message_body(uint8_t* message_body, int message_body_length, MessageHeader* header, field_t* fields, size_t* fields_used) {
    BAIL_IF(header->instructions_length != 1);

    Parser parser = {message_body, message_body_length};
    Instruction instruction;
    BAIL_IF(parse_instruction(&parser, &instruction));
    BAIL_IF(instruction_validate(&instruction, header));

    InstructionInfo info;
    memset(&info, 0, sizeof(InstructionInfo));
    enum ProgramId program_id = instruction_program_id(&instruction, header);
    switch (program_id) {
        case ProgramIdSystem:
        {
            if (parse_system_instructions(&instruction, header, &info.system) == 0) {
                info.kind = program_id;
            }
            break;
        }
        case ProgramIdStake:
        {
            if (parse_stake_instructions(&instruction, header, &info.stake) == 0) {
                info.kind = program_id;
            }
            break;
        }
        case ProgramIdUnknown:
            break;
    }

    // If we don't know about the instruction, bail
    BAIL_IF(info.kind == ProgramIdUnknown);

    switch (info.kind) {
        case ProgramIdSystem:
            return print_system_info(&info.system, header, fields, fields_used);
        case ProgramIdStake:
            return print_stake_info(&info.stake, header, fields, fields_used);
        case ProgramIdUnknown:
            break;
    }

    *fields_used = 0;
    return 1;
}
