#include "instruction.h"
#include "sol/parser.h"
#include "sol/message.h"
#include "system_instruction.h"
#include "stake_instruction.h"
#include "transaction_printers.h"
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

    return print_transaction(header, instruction_info, instruction_count);
}
