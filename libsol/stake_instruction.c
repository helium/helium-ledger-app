#include "common_byte_strings.h"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
#include "stake_instruction.h"
#include "util.h"
#include <string.h>

const Pubkey stake_program_id = {{
    PROGRAM_ID_STAKE
}};

static int parse_stake_instruction_kind(Parser* parser, enum StakeInstructionKind* kind) {
    uint32_t maybe_kind;
    BAIL_IF(parse_u32(parser, &maybe_kind));
    switch (maybe_kind) {
        case StakeInitialize:
        case StakeAuthorize:
        case StakeDelegate:
        case StakeSplit:
        case StakeWithdraw:
        case StakeDeactivate:
        case StakeSetLockup:
            *kind = (enum StakeInstructionKind) maybe_kind;
            return 0;
    }
    return 1;
}

// Returns 0 and populates StakeDelegateInfo if provided a MessageHeader and a delegate
// instruction, otherwise non-zero.
static int parse_delegate_stake_instruction(Instruction* instruction, Pubkey* pubkeys, size_t pubkeys_length, StakeDelegateInfo* info) {
    BAIL_IF(instruction->accounts_length < 6);
    uint8_t accounts_index = 0;
    uint8_t pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    info->stake_pubkey = &pubkeys[pubkeys_index];

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    info->vote_pubkey = &pubkeys[pubkeys_index];

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    //Pubkey* pubkey = &pubkeys[pubkeys_index];
    //BAIL_IF(memcmp(pubkey, &clock_pubkey, PUBKEY_SIZE));

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    //pubkey = &pubkeys[pubkeys_index];
    //BAIL_IF(memcmp(pubkey, &stake_history_pubkey, PUBKEY_SIZE));

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    //pubkey = &pubkeys[pubkeys_index];
    //BAIL_IF(memcmp(pubkey, &config_pubkey, PUBKEY_SIZE));

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    info->authorized_pubkey = &pubkeys[pubkeys_index];

    return 0;
}

int parse_stake_instructions(Instruction* instruction, MessageHeader* header, StakeInfo* info) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_stake_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case StakeDelegate:
            return parse_delegate_stake_instruction(instruction, header->pubkeys, header->pubkeys_header.pubkeys_length, &info->delegate_stake);
        case StakeInitialize:
        case StakeAuthorize:
        case StakeSplit:
        case StakeWithdraw:
        case StakeDeactivate:
        case StakeSetLockup:
            break;
    }

    return 1;
}

static int print_delegate_stake_info(StakeDelegateInfo* info, MessageHeader* header) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_pubkey(item, "Delegate from", info->stake_pubkey);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Authorized by", info->authorized_pubkey);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Vote account", info->vote_pubkey);

    item = transaction_summary_fee_payer_item();
    if (memcmp(&header->pubkeys[0], info->authorized_pubkey, PUBKEY_SIZE) == 0) {
        transaction_summary_set_fee_payer_string("authorizer");
    }

    return 0;
}

int print_stake_info(StakeInfo* info, MessageHeader* header) {
    switch (info->kind) {
        case StakeDelegate:
            return print_delegate_stake_info(&info->delegate_stake, header);
        case StakeInitialize:
        case StakeAuthorize:
        case StakeSplit:
        case StakeWithdraw:
        case StakeDeactivate:
        case StakeSetLockup:
            break;
    }

    return 1;
}
