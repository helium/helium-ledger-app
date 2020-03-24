#include "instruction.h"

#define BAIL_IF(x) {int err = x; if (err) return err;}

int instruction_validate(const Instruction* instruction, const MessageHeader* header) {
    BAIL_IF(instruction->program_id_index >= header->pubkeys_header.pubkeys_length);
    for (size_t i = 0; i < instruction->accounts_length; i++) {
        BAIL_IF(instruction->accounts[i] >= header->pubkeys_header.pubkeys_length);
    }
    return 0;
}
