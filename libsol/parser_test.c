#include "instruction.h"
#include "parser.c"
#include <stdio.h>
#include <assert.h>

void test_parse_u8() {
   uint8_t message[] = {1, 2};
   Parser parser = {message, sizeof(message)};
   uint8_t value;
   assert(parse_u8(&parser, &value) == 0);
   assert(parser.buffer_length == 1);
   assert(parser.buffer == message + 1);
   assert(value == 1);
}

void test_parse_u8_too_short() {
   uint8_t message[] = {42};
   Parser parser = {message, sizeof(message)};
   uint8_t value;
   assert(parse_u8(&parser, &value) == 0);
   assert(parse_u8(&parser, &value) == 1);
}

void test_parse_u16() {
   uint8_t message[] = {0, 0, 255, 255};
   Parser parser = {message, sizeof(message)};
   uint16_t value;
   assert(parse_u16(&parser, &value) == 0);
   assert(value == 0);
   assert(parse_u16(&parser, &value) == 0);
   assert(value == UINT16_MAX);
   assert(parser_is_empty(&parser));
}

void test_parse_u32() {
   uint8_t message[] = {0, 0, 0, 0, 255, 255, 255, 255};
   Parser parser = {message, sizeof(message)};
   uint32_t value;
   assert(parse_u32(&parser, &value) == 0);
   assert(value == 0);
   assert(parse_u32(&parser, &value) == 0);
   assert(value == UINT32_MAX);
   assert(parser_is_empty(&parser));
}

void test_parse_u64() {
   uint8_t message[] = {
      0, 0, 0, 0, 0, 0, 0, 0,
      255, 255, 255, 255, 255, 255, 255, 255
   };
   Parser parser = {message, sizeof(message)};
   uint64_t value;
   assert(parse_u64(&parser, &value) == 0);
   assert(value == 0);
   assert(parse_u64(&parser, &value) == 0);
   assert(value == UINT64_MAX);
   assert(parser_is_empty(&parser));
}

void test_parse_length() {
   uint8_t message[] = {1, 2};
   Parser parser = {message, sizeof(message)};
   size_t value;
   assert(parse_length(&parser, &value) == 0);
   assert(parser.buffer_length == 1);
   assert(parser.buffer == message + 1);
   assert(value == 1);
}

void test_parse_length_two_bytes() {
   uint8_t message[] = {128, 1};
   Parser parser = {message, sizeof(message)};
   size_t value;
   assert(parse_length(&parser, &value) == 0);
   assert(parser_is_empty(&parser));
   assert(parser.buffer == message + 2);
   assert(value == 128);
}

void test_parse_pubkeys_header() {
   uint8_t message[] = {1, 2, 3, 4};
   Parser parser = {message, sizeof(message)};
   PubkeysHeader header;
   assert(parse_pubkeys_header(&parser, &header) == 0);
   assert(parser_is_empty(&parser));
   assert(parser.buffer == message + 4);
   assert(header.pubkeys_length == 4);
}

void test_parse_pubkeys() {
   uint8_t message[PUBKEY_SIZE + 4] = {1, 2, 3, 1, 42};
   Parser parser = {message, sizeof(message)};
   PubkeysHeader header;
   Pubkey* pubkeys;
   assert(parse_pubkeys(&parser, &header, &pubkeys) == 0);
   assert(parser_is_empty(&parser));
   assert(parser.buffer == message + PUBKEY_SIZE + 4);
   assert(pubkeys->data[0] == 42);
}

void test_parse_pubkeys_too_short() {
   uint8_t message[] = {1, 2, 3, 1};
   Parser parser = {message, sizeof(message)};
   PubkeysHeader header;
   Pubkey* pubkeys;
   assert(parse_pubkeys(&parser, &header, &pubkeys) == 1);
}

void test_parse_hash() {
   uint8_t message[HASH_SIZE] = {42};
   Parser parser = {message, sizeof(message)};
   Hash* hash;
   assert(parse_hash(&parser, &hash) == 0);
   assert(parser_is_empty(&parser));
   assert(parser.buffer == message + HASH_SIZE);
   assert(hash->data[0] == 42);
}

void test_parse_hash_too_short() {
   uint8_t message[31]; // <--- Too short!
   Parser parser = {message, sizeof(message)};
   Hash* hash;
   assert(parse_hash(&parser, &hash) == 1);
}

void test_parse_data() {
   uint8_t message[] = {1, 2};
   Parser parser = {message, sizeof(message)};
   uint8_t* data;
   size_t data_length;
   assert(parse_data(&parser, &data, &data_length) == 0);
   assert(parser_is_empty(&parser));
   assert(parser.buffer == message + 2);
   assert(data[0] == 2);
}

void test_parse_data_too_short() {
   uint8_t message[] = {1}; // <--- Too short!
   Parser parser = {message, sizeof(message)};
   uint8_t* data;
   size_t data_length;
   assert(parse_data(&parser, &data, &data_length) == 1);
}

void test_parse_instruction() {
   uint8_t message[] = {0, 2, 33, 34, 1, 36};
   Parser parser = {message, sizeof(message)};
   Instruction instruction;
   assert(parse_instruction(&parser, &instruction) == 0);
   MessageHeader header = {{0, 0, 0, 35}, NULL, NULL, 1};
   assert(instruction_validate(&instruction, &header) == 0);
   assert(parser_is_empty(&parser));
   assert(instruction.accounts[0] == 33);
   assert(instruction.data[0] == 36);
}

void test_parser_is_empty() {
    uint8_t buf[1] = {0};
    Parser nonempty = {buf, 1};
    assert(!parser_is_empty(&nonempty));
    Parser empty = {NULL, 0};
    assert(parser_is_empty(&empty));
}

int main() {
    test_parse_u8();
    test_parse_u8_too_short();
    test_parse_u16();
    test_parse_u32();
    test_parse_u64();
    test_parse_length();
    test_parse_length_two_bytes();
    test_parse_pubkeys_header();
    test_parse_pubkeys();
    test_parse_pubkeys_too_short();
    test_parse_hash();
    test_parse_hash_too_short();
    test_parse_data();
    test_parse_data_too_short();
    test_parse_instruction();
    test_parser_is_empty();

    printf("passed\n");
    return 0;
}
