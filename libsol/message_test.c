#include "message.c"
#include "sol/printer.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>

void test_process_message_body_ok() {
    Pubkey accounts[] = {
        {{171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218}},
        {{204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54}},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    };
    Blockhash blockhash = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    MessageHeader header = {{1, 0, 1, 3}, accounts, &blockhash, 1};
    uint8_t msg_body[] = {2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};
    field_t fields[5];
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, fields, &fields_used) == 0);
    assert(fields_used == 4);
}

void test_process_message_body_xfer_w_nonce_ok() {
    Pubkey accounts[] = {
        {{171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218}},
        {{204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54}},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    };
    Blockhash blockhash = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    MessageHeader header = {{1, 0, 1, 3}, accounts, &blockhash, 2};
    uint8_t msg_body[] = {
        2, 3, 0, 1, 0, 4, 4, 0, 0, 0, // Nonce
        2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0
    };
    field_t fields[6];
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, fields, &fields_used) == 0);
    assert(fields_used == 6);
}

void test_process_message_body_too_few_ix_fail() {
    MessageHeader header = {{0, 0, 0, 0}, NULL, NULL, 0};
    size_t fields_used = 0;
    assert(process_message_body(NULL, 0, &header, NULL, &fields_used) == 1);
}

void test_process_message_body_too_many_ix_fail() {
    Pubkey accounts[] = {
        {{171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218}},
        {{204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54}},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    };
    Blockhash blockhash = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    uint8_t xfer_ix[] = {2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0};

#define TOO_MANY_IX (MAX_INSTRUCTIONS + 1)
#define XFER_IX_LEN ARRAY_LEN(xfer_ix)

    uint8_t msg_body[TOO_MANY_IX * XFER_IX_LEN];
    for (size_t i = 0; i < TOO_MANY_IX; i++) {
        uint8_t* start = msg_body + (i * XFER_IX_LEN);
        memcpy(start, xfer_ix, XFER_IX_LEN);
    }
    MessageHeader header = {{1, 0, 1, 3}, accounts, &blockhash, TOO_MANY_IX};
    field_t fields[5];
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, fields, &fields_used) == 1);
}

void test_process_message_body_data_too_short_fail() {
    MessageHeader header = {{0, 0, 0, 0}, NULL, NULL, 1};
    size_t fields_used = 0;
    assert(process_message_body(NULL, 0, &header, NULL, &fields_used) == 1);
}

void test_process_message_body_data_too_long_fail() {
    Pubkey accounts[] = {
        {{171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218}},
        {{204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54}},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    };
    Blockhash blockhash = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    MessageHeader header = {{1, 0, 1, 3}, accounts, &blockhash, 1};
    uint8_t msg_body[] = {
        2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0,
        0
    };
    field_t fields[5];
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, fields, &fields_used) == 1);
}

void test_process_message_body_bad_ix_account_index_fail() {
    MessageHeader header = {{0, 0, 0, 1}, NULL, NULL, 1};
    uint8_t msg_body[] = {1, 0, 0};
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, NULL, &fields_used) == 1);
}

void test_process_message_body_unknown_ix_enum_fail() {
    Pubkey accounts[] = {
        {{171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218}},
        {{204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54}},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    };
    Blockhash blockhash = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    MessageHeader header = {{1, 0, 1, 3}, accounts, &blockhash, 1};
    uint8_t msg_body[] = {
        2, 2, 0, 1, 12, 255, 255, 255, 255, 42, 0, 0, 0, 0, 0, 0, 0,
    };
    field_t fields[5];
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, fields, &fields_used) == 1);
}

void test_process_message_body_ix_with_unknown_program_id_fail() {
    Pubkey accounts[] = {
        {{171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218}},
        {{204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54}},
        {{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}},
    };
    Blockhash blockhash = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    MessageHeader header = {{1, 0, 1, 3}, accounts, &blockhash, 1};
    uint8_t msg_body[] = {
        2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0,
    };
    field_t fields[5];
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, fields, &fields_used) == 1);
}

int main() {
    test_process_message_body_ok();
    test_process_message_body_too_few_ix_fail();
    test_process_message_body_too_many_ix_fail();
    test_process_message_body_data_too_short_fail();
    test_process_message_body_data_too_long_fail();
    test_process_message_body_bad_ix_account_index_fail();
    test_process_message_body_unknown_ix_enum_fail();
    test_process_message_body_ix_with_unknown_program_id_fail();
    test_process_message_body_xfer_w_nonce_ok();

    printf("passed\n");
    return 0;
}
