#include "sol/parser.h"
#include "sol/printer.h"
#include "sol/message.h"
#include "system_instruction.h"
#include "stake_instruction.h"
#include <string.h>

#define BAIL_IF(x) {int err = x; if (err) return err;}

int process_message_body(uint8_t* message_body, int message_body_length, MessageHeader* header, field_t* fields, size_t* fields_used) {
    BAIL_IF(header->instructions_length != 1);

    Parser parser = {message_body, message_body_length};
    Instruction instruction;
    BAIL_IF(parse_instruction(&parser, &instruction));

    const Pubkey* program_id = &header->pubkeys[instruction.program_id_index];
    if (memcmp(program_id, &system_program_id, PUBKEY_SIZE) == 0) {
        SystemTransferInfo transfer_info;
        if (parse_system_transfer_instructions(&parser, &instruction, header, &transfer_info) == 0) {
            return print_system_transfer_info(&transfer_info, header, fields, fields_used);
        }
    }
    else if (memcmp(program_id, &stake_program_id, PUBKEY_SIZE) == 0) {
        DelegateStakeInfo delegate_info;
        if (parse_delegate_stake_instructions(&parser, &instruction, header, &delegate_info) == 0) {
            return print_delegate_stake_info(&delegate_info, header, fields, fields_used);
        }
    }

    *fields_used = 0;
    return 1;
}
