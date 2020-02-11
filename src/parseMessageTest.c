#ifdef UNIT_TEST

#include "parseMessage.c"
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
   uint8_t message[] = {};
   Parser parser = {message, sizeof(message)};
   uint8_t value;
   assert(parse_u8(&parser, &value) == 1);
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
   assert(parser.buffer_length == 0);
   assert(parser.buffer == message + 2);
   assert(value == 128);
}

void test_parse_message_header() {
   uint8_t message[] = {1, 2, 3};
   Parser parser = {message, sizeof(message)};
   MessageHeader header;
   assert(parse_message_header(&parser, &header) == 0);
   assert(parser.buffer_length == 0);
   assert(parser.buffer == message + 3);
}

void test_parse_pubkeys() {
   uint8_t message[PUBKEY_SIZE + 1] = {1, 42};
   Parser parser = {message, sizeof(message)};
   Pubkey* pubkeys;
   size_t pubkeys_length;
   assert(parse_pubkeys(&parser, &pubkeys, &pubkeys_length) == 0);
   assert(parser.buffer_length == 0);
   assert(parser.buffer == message + PUBKEY_SIZE + 1);
   assert(pubkeys->data[0] == 42);
}

void test_parse_pubkeys_too_short() {
   uint8_t message[] = {1};
   Parser parser = {message, sizeof(message)};
   Pubkey* pubkeys;
   size_t pubkeys_length;
   assert(parse_pubkeys(&parser, &pubkeys, &pubkeys_length) == 1);
}

void test_parse_blockhash() {
   uint8_t message[BLOCKHASH_SIZE] = {42};
   Parser parser = {message, sizeof(message)};
   Blockhash* blockhash;
   assert(parse_blockhash(&parser, &blockhash) == 0);
   assert(parser.buffer_length == 0);
   assert(parser.buffer == message + BLOCKHASH_SIZE);
   assert(blockhash->data[0] == 42);
}

void test_parse_blockhash_too_short() {
   uint8_t message[31]; // <--- Too short!
   Parser parser = {message, sizeof(message)};
   Blockhash* blockhash;
   assert(parse_blockhash(&parser, &blockhash) == 1);
}

void test_parse_data() {
   uint8_t message[] = {1, 2};
   Parser parser = {message, sizeof(message)};
   uint8_t* data;
   size_t data_length;
   assert(parse_data(&parser, &data, &data_length) == 0);
   assert(parser.buffer_length == 0);
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
   assert(parser.buffer_length == 0);
   assert(instruction.accounts[0] == 33);
   assert(instruction.data[0] == 36);
}

int main() {
    test_parse_u8();
    test_parse_u8_too_short();
    test_parse_length();
    test_parse_length_two_bytes();
    test_parse_message_header();
    test_parse_pubkeys();
    test_parse_pubkeys_too_short();
    test_parse_blockhash();
    test_parse_blockhash_too_short();
    test_parse_data();
    test_parse_data_too_short();
    test_parse_instruction();

    printf("passed\n");
    return 0;
}

#endif
