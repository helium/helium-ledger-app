#include "common_byte_strings.h"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
#include "stake_instruction.h"
#include "util.h"
#include <stdbool.h>
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
static int parse_delegate_stake_instruction(const Instruction* instruction, const MessageHeader* header, StakeDelegateInfo* info) {
    BAIL_IF(instruction->accounts_length < 6);
    size_t pubkeys_length = header->pubkeys_header.pubkeys_length;
    uint8_t accounts_index = 0;
    uint8_t pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    info->stake_pubkey = &header->pubkeys[pubkeys_index];

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    info->vote_pubkey = &header->pubkeys[pubkeys_index];

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    //const Pubkey* pubkey = &header->pubkeys[pubkeys_index];
    //BAIL_IF(memcmp(pubkey, &clock_pubkey, PUBKEY_SIZE));

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    //pubkey = &header->pubkeys[pubkeys_index];
    //BAIL_IF(memcmp(pubkey, &stake_history_pubkey, PUBKEY_SIZE));

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    //pubkey = &header-:pubkeys[pubkeys_index];
    //BAIL_IF(memcmp(pubkey, &config_pubkey, PUBKEY_SIZE));

    pubkeys_index = instruction->accounts[accounts_index++];
    BAIL_IF(pubkeys_index >= pubkeys_length);
    info->authorized_pubkey = &header->pubkeys[pubkeys_index];

    return 0;
}

static int parse_stake_initialize_instruction(Parser* parser, const Instruction* instruction, const MessageHeader* header, StakeInitializeInfo* info) {
    BAIL_IF(instruction->accounts_length < 2);
    size_t accounts_index = 0;
    uint8_t pubkeys_index = instruction->accounts[accounts_index++];
    info->account = &header->pubkeys[pubkeys_index];

    accounts_index++; // Skip rent sysvar

    BAIL_IF(parse_pubkey(parser, &info->stake_authority));
    BAIL_IF(parse_pubkey(parser, &info->withdraw_authority));

    // Lockup
    BAIL_IF(parse_i64(parser, &info->lockup.unix_timestamp));
    BAIL_IF(parse_u64(parser, &info->lockup.epoch));
    BAIL_IF(parse_pubkey(parser, &info->lockup.custodian));
    info->lockup. present = StakeLockupHasAll;

    return 0;
}

int parse_stake_instructions(const Instruction* instruction, const MessageHeader* header, StakeInfo* info) {
    Parser parser = {instruction->data, instruction->data_length};

    BAIL_IF(parse_stake_instruction_kind(&parser, &info->kind));

    switch (info->kind) {
        case StakeDelegate:
            return parse_delegate_stake_instruction(instruction, header, &info->delegate_stake);
        case StakeInitialize:
            return parse_stake_initialize_instruction(&parser, instruction, header, &info->initialize);
        case StakeAuthorize:
        case StakeSplit:
        case StakeWithdraw:
        case StakeDeactivate:
        case StakeSetLockup:
            break;
    }

    return 1;
}

static int print_delegate_stake_info(const StakeDelegateInfo* info, const MessageHeader* header) {
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

int print_stake_info(const StakeInfo* info, const MessageHeader* header) {
    switch (info->kind) {
        case StakeDelegate:
            return print_delegate_stake_info(&info->delegate_stake, header);
        case StakeInitialize:
            return print_stake_initialize_info("Init. stake acct", &info->initialize, header);
        case StakeAuthorize:
        case StakeSplit:
        case StakeWithdraw:
        case StakeDeactivate:
        case StakeSetLockup:
            break;
    }

    return 1;
}

int print_stake_initialize_info(
    const char* primary_title,
    const StakeInitializeInfo* info,
    const MessageHeader* header
) {
    SummaryItem* item;
    bool one_authority = pubkeys_equal(
        info->withdraw_authority,
        info->stake_authority
    );

    if (primary_title != NULL) {
        item = transaction_summary_primary_item();
        summary_item_set_pubkey(item, primary_title, info->account);
    }

    if (one_authority) {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "New authority", info->stake_authority);
    } else {
        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "New stake auth", info->stake_authority);

        item = transaction_summary_general_item();
        summary_item_set_pubkey(item, "New withdraw auth", info->withdraw_authority);
    }

    item = transaction_summary_general_item();
    summary_item_set_i64(item, "Lockup time", info->lockup.unix_timestamp);

    item = transaction_summary_general_item();
    summary_item_set_u64(item, "Lockup epoch", info->lockup.epoch);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Lockup custodian", info->lockup.custodian);

    return 0;
}
