#include "common_byte_strings.h"
#include "instruction.h"
#include "serum_assert_owner_instruction.h"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
#include "util.h"

const Pubkey serum_assert_owner_program_id = {{
    PROGRAM_ID_SERUM_ASSERT_OWNER
}};

static int parse_serum_assert_owner_check_instruction(
    Parser* parser,
    const Instruction* instruction,
    const MessageHeader* header,
    SerumAssertOwnerCheckInfo* info
) {
    InstructionAccountsIterator it;
    instruction_accounts_iterator_init(&it, header, instruction);

    BAIL_IF(instruction_accounts_iterator_next(&it, &info->account));

    BAIL_IF(parse_pubkey(parser, &info->expected_owner));

    return 0;
}

int parse_serum_assert_owner_instructions(
    const Instruction* instruction,
    const MessageHeader* header,
    SerumAssertOwnerInfo* info
) {
    Parser parser = {instruction->data, instruction->data_length};
    return parse_serum_assert_owner_check_instruction(
        &parser,
        instruction,
        header,
        &info->check
    );
}

