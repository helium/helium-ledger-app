#include "sol/parser.h"
#include "sol/printer.h"
#include "stake_instruction.h"
#include "util.h"
#include <string.h>

// Stake11111111111111111111111111111111111111
const Pubkey stake_program_id = {{
0x06, 0xa1, 0xd8, 0x17, 0x91, 0x37, 0x54, 0x2a, 0x98, 0x34,
0x37, 0xbd, 0xfe, 0x2a, 0x7a, 0xb2, 0x55, 0x7f, 0x53, 0x5c,
0x8a, 0x78, 0x72, 0x2b, 0x68, 0xa4, 0x9d, 0xc0, 0x00, 0x00,
0x00, 0x00
}};

static int parse_stake_instruction_kind(Parser* parser, enum StakeInstructionKind* kind) {
    uint32_t maybe_kind;
    BAIL_IF(parse_u32(parser, &maybe_kind));
    switch (maybe_kind) {
        case Initialize:
        case Authorize:
        case DelegateStake:
        case Split:
        case Withdraw:
        case Deactivate:
        case SetLockup:
            *kind = (enum StakeInstructionKind) maybe_kind;
            return 0;
    }
    return 1;
}

// Returns 0 and populates DelegateStakeInfo if provided a MessageHeader and a delegate
// instruction, otherwise non-zero.
static int parse_delegate_stake_instruction(Instruction* instruction, Pubkey* pubkeys, size_t pubkeys_length, DelegateStakeInfo* info) {
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
        case DelegateStake:
            return parse_delegate_stake_instruction(instruction, header->pubkeys, header->pubkeys_header.pubkeys_length, &info->delegate_stake);
        case Initialize:
        case Authorize:
        case Split:
        case Withdraw:
        case Deactivate:
        case SetLockup:
            break;
    }

    return 1;
}

static int print_delegate_stake_info(DelegateStakeInfo* info, MessageHeader* header, field_t* fields, size_t* fields_used) {
    char pubkey_buffer[BASE58_PUBKEY_LENGTH];
    strcpy(fields[0].title, "Delegate from");
    encode_base58((uint8_t*) info->stake_pubkey, PUBKEY_SIZE, (uint8_t*) pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[0].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

    strcpy(fields[1].title, "Authorized by");
    encode_base58((uint8_t*) info->authorized_pubkey, PUBKEY_SIZE, (uint8_t*) pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[1].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

    strcpy(fields[2].title, "Vote account");
    encode_base58((uint8_t*) info->vote_pubkey, PUBKEY_SIZE, (uint8_t*) pubkey_buffer, BASE58_PUBKEY_LENGTH);
    print_summary(pubkey_buffer, fields[2].text, SUMMARY_LENGTH, SUMMARY_LENGTH);

    if (memcmp(&header->pubkeys[0], info->authorized_pubkey, PUBKEY_SIZE) == 0) {
        strcpy(fields[3].text, "authorizer");
    }

    *fields_used = 4;
    return 0;
}

int print_stake_info(StakeInfo* info, MessageHeader* header, field_t*  fields, size_t* fields_used) {
    switch (info->kind) {
        case DelegateStake:
            return print_delegate_stake_info(&info->delegate_stake, header, fields, fields_used);
        case Initialize:
        case Authorize:
        case Split:
        case Withdraw:
        case Deactivate:
        case SetLockup:
            break;
    }

    return 1;
}
