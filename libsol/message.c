#include "sol/parser.h"
#include "sol/printer.h"
#include "sol/message.h"
#include "system_instruction.h"
#include "stake_instruction.h"

int process_message_body(uint8_t* message_body, int message_body_length, MessageHeader* header, field_t* fields, size_t* fields_used) {
    {
        // Check if these are system instructions
        Parser parser = {message_body, message_body_length};
        SystemTransferInfo transfer_info;
        if (parse_system_transfer_instructions(&parser, header, &transfer_info) == 0) {
            return print_system_transfer_info(&transfer_info, header, fields, fields_used);
        }
    }

    {
        // Check if these are staking instructions
        Parser parser = {message_body, message_body_length};
        DelegateStakeInfo delegate_info;
        if (parse_delegate_stake_instructions(&parser, header, &delegate_info) == 0) {
            return print_delegate_stake_info(&delegate_info, header, fields, fields_used);
        }
    }

    *fields_used = 0;
    return 1;
}
