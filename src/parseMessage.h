#ifndef PARSE_MESSAGE_H
#define PARSE_MESSAGE_H

#include <stdint.h>
#include <stddef.h>

typedef struct MessageParser {
    uint8_t* buffer;
    size_t buffer_length;
} MessageParser;

typedef struct Pubkey {
    uint8_t data[32];
} Pubkey;

typedef struct Blockhash {
    uint8_t data[32];
} Blockhash;

typedef struct Instruction {
    uint8_t program_id_index;
    uint8_t* accounts;
    size_t accounts_length;
    uint8_t* data;
    size_t data_length;
} Instruction;

typedef struct MessageHeader {
    uint8_t num_required_signatures;
    uint8_t num_readonly_signed_accounts;
    uint8_t num_readonly_unsigned_accounts;
} MessageHeader;

int parse_length(MessageParser* parser, size_t* value);

int parse_message_header(MessageParser* parser, MessageHeader* header);

int parse_pubkeys(MessageParser* parser, Pubkey** pubkeys, size_t* pubkeys_length);

int parse_blockhash(MessageParser* parser, Blockhash** blockhash);

int parse_instruction(MessageParser* parser, Instruction* instruction);

#endif // PARSE_MESSAGE_H
