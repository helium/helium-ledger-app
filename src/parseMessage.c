#include "parseMessage.h"

#define PUBKEY_LENGTH 32
#define HASH_LENGTH 32
#define BAIL_IF(x) {int err = x; if (err) return err;}

static int check_buffer_length(MessageParser* parser, size_t num) {
    return parser->buffer_length < num ? 1 : 0;
}

static void advance(MessageParser* parser, size_t num) {
    parser->buffer += num;
    parser->buffer_length -= num;
}

static int parse_u8(MessageParser* parser, uint8_t* value) {
    BAIL_IF(check_buffer_length(parser, 1));
    *value = *parser->buffer;
    advance(parser, 1);
    return 0;
}

int parse_length(MessageParser* parser, size_t* value) {
    uint8_t value_u8;
    BAIL_IF(parse_u8(parser, &value_u8));
    *value = value_u8 & 0x7f;

    if (value_u8 & 0x80) {
        BAIL_IF(parse_u8(parser, &value_u8));
        *value = ((value_u8 & 0x7f) << 7) | *value;
        if (value_u8 & 0x80) {
            BAIL_IF(parse_u8(parser, &value_u8));
            *value = ((value_u8 & 0x7f) << 14) | *value;
	}
    }
    return 0;
}

int parse_message_header(MessageParser* parser, MessageHeader* header) {
    BAIL_IF(parse_u8(parser, &header->num_required_signatures));
    BAIL_IF(parse_u8(parser, &header->num_readonly_signed_accounts));
    BAIL_IF(parse_u8(parser, &header->num_readonly_unsigned_accounts));
    return 0;
}

int parse_pubkeys(MessageParser* parser, Pubkey** pubkeys, size_t* pubkeys_length) {
    BAIL_IF(parse_length(parser, pubkeys_length));
    BAIL_IF(check_buffer_length(parser, *pubkeys_length * PUBKEY_LENGTH));
    *pubkeys = (Pubkey*) parser->buffer;
    advance(parser, *pubkeys_length * PUBKEY_LENGTH);
    return 0;
}

int parse_blockhash(MessageParser* parser, Blockhash** blockhash) {
    BAIL_IF(check_buffer_length(parser, HASH_LENGTH));
    *blockhash = (Blockhash*) parser->buffer;
    advance(parser, HASH_LENGTH);
    return 0;
}

static int parse_data(MessageParser* parser, uint8_t** data, size_t* data_length) {
    BAIL_IF(parse_length(parser, data_length));
    BAIL_IF(check_buffer_length(parser, *data_length));
    *data = parser->buffer;
    advance(parser, *data_length);
    return 0;
}

int parse_instruction(MessageParser* parser, Instruction* instruction) {
    BAIL_IF(parse_u8(parser, &instruction->program_id_index));
    BAIL_IF(parse_data(parser, &instruction->accounts, &instruction->accounts_length));
    BAIL_IF(parse_data(parser, &instruction->data, &instruction->data_length));
    return 0;
}
