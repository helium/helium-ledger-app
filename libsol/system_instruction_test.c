#include "system_instruction.c"
#include <stdio.h>
#include <assert.h>

#define assert_string_equal(actual, expected) assert(strcmp(actual, expected) == 0)

void test_parse_system_transfer_instructions() {
    uint8_t message[] = {1, 0, 1, 3, 171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218, 204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Pubkey* fee_payer_pubkey = &header.pubkeys[0];
    SystemTransferInfo info;
    assert(parse_system_transfer_instructions(&parser, &header, &info) == 0);
    assert(parser.buffer_length == 0);
    assert(info.lamports == 42);
    assert(memcmp(fee_payer_pubkey, info.from, PUBKEY_SIZE) == 0);
}

void test_parse_system_transfer_instructions_with_payer() {
    uint8_t message[] = {2, 0, 1, 3, 204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54, 171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Pubkey* fee_payer_pubkey = &header.pubkeys[0];
    SystemTransferInfo info;
    assert(parse_system_transfer_instructions(&parser, &header, &info) == 0);

    // "to", not "from", is paying for this transaction.
    assert(memcmp(fee_payer_pubkey, info.to, PUBKEY_SIZE) == 0);
}


void test_process_system_transfer() {
    uint8_t message[] = {1, 0, 1, 3, 171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218, 204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    field_t fields[5];
    size_t fields_used;
    SystemTransferInfo info;
    assert(parse_system_transfer_instructions(&parser, &header, &info) == 0);
    assert(print_system_transfer_info(&info, &header, fields, &fields_used) == 0);
    assert_string_equal(fields[0].text, "0.000000042 SOL");

    // Fee-payer is sender
    assert_string_equal(fields[3].text, "sender");
}

int main() {
    test_parse_system_transfer_instructions();
    test_parse_system_transfer_instructions_with_payer();
    test_process_system_transfer();

    printf("passed\n");
    return 0;
}
