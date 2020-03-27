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

void test_process_message_body_too_many_ix_fail() {
    Pubkey accounts[] = {
        {{171, 88, 202, 32, 185, 160, 182, 116, 130, 185, 73, 48, 13, 216, 170, 71, 172, 195, 165, 123, 87, 70, 130, 219, 5, 157, 240, 187, 26, 191, 158, 218}},
        {{204, 241, 115, 109, 41, 173, 110, 48, 24, 113, 210, 213, 163, 78, 1, 112, 146, 114, 235, 220, 96, 185, 184, 85, 163, 27, 124, 48, 54, 250, 233, 54}},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    };
    Blockhash blockhash = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    uint8_t msg_body[] = {
        2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 0, 1, 12, 2, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0,
    };
    MessageHeader header = {{1, 0, 1, 3}, accounts, &blockhash, 2};
    field_t fields[5];
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, fields, &fields_used) == 1);
}

void test_process_message_body_too_short_ix_fail() {
    MessageHeader header = {{0, 0, 0, 0}, NULL, NULL, 1};
    size_t fields_used = 0;
    assert(process_message_body(NULL, 0, &header, NULL, &fields_used) == 1);
}

void test_process_message_body_bad_ix_account_index_fail() {
    MessageHeader header = {{0, 0, 0, 1}, NULL, NULL, 1};
    uint8_t msg_body[] = {1, 0, 0};
    size_t fields_used = 0;
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header, NULL, &fields_used) == 1);
}

int main() {
    test_process_message_body_ok();
    test_process_message_body_too_many_ix_fail();
    test_process_message_body_too_short_ix_fail();
    test_process_message_body_bad_ix_account_index_fail();

    printf("passed\n");
    return 0;
}
