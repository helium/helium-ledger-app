#include "message.c"
#include "sol/parser.h"
#include "sol/transaction_summary.h"
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

    transaction_summary_reset();
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header) == 0);
    enum SummaryItemKind kinds[MAX_TRANSACTION_SUMMARY_ITEMS];
    size_t num_kinds;
    assert(transaction_summary_finalize(kinds, &num_kinds) == 0);
    assert(num_kinds == 4);
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
    transaction_summary_reset();
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header) == 0);
    enum SummaryItemKind kinds[MAX_TRANSACTION_SUMMARY_ITEMS];
    size_t num_kinds;
    assert(transaction_summary_finalize(kinds, &num_kinds) == 0);
    assert(num_kinds == 6);
}

void test_process_message_body_too_few_ix_fail() {
    MessageHeader header = {{0, 0, 0, 0}, NULL, NULL, 0};
    assert(process_message_body(NULL, 0, &header) == 1);
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
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header) == 1);
}

void test_process_message_body_data_too_short_fail() {
    MessageHeader header = {{0, 0, 0, 0}, NULL, NULL, 1};
    assert(process_message_body(NULL, 0, &header) == 1);
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
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header) == 1);
}

void test_process_message_body_bad_ix_account_index_fail() {
    MessageHeader header = {{0, 0, 0, 1}, NULL, NULL, 1};
    uint8_t msg_body[] = {1, 0, 0};
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header) == 1);
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
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header) == 1);
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
    assert(process_message_body(msg_body, ARRAY_LEN(msg_body), &header) == 1);
}

void test_process_message_body_nonced_stake_create_with_seed() {
    uint8_t message[] = {
        2, 1, 4, 8,
            18, 67, 85, 168, 124, 173, 88, 142, 77, 171, 80, 178, 8, 218, 230, 68, 85, 231, 39, 54, 184, 42, 162, 85, 172, 139, 54, 173, 194, 7, 64, 250,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            247, 157, 35, 131, 179, 105, 135, 105, 0, 178, 6, 62, 22, 251, 47, 102, 208, 237, 66, 72, 149, 5, 127, 149, 253, 28, 66, 250, 52, 173, 30, 105,
            6, 167, 213, 23, 25, 44, 86, 142, 224, 138, 132, 95, 115, 210, 151, 136, 207, 3, 92, 49, 69, 178, 26, 179, 68, 216, 6, 46, 169, 64, 0, 0,
            6, 167, 213, 23, 25, 44, 92, 81, 33, 140, 201, 76, 61, 74, 241, 127, 88, 218, 238, 8, 155, 161, 253, 68, 227, 219, 217, 138, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            6, 161, 216, 23, 145, 55, 84, 42, 152, 52, 55, 189, 254, 42, 122, 178, 85, 127, 83, 92, 138, 120, 114, 43, 104, 164, 157, 192, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        3,
            /* nonce */
            6,
            3,
                2, 4, 1,
            4,
                4, 0, 0, 0,
            /* create w/ seed */
            6,
            2,
                0, 3,
            124,
                3, 0, 0, 0,
                18, 67, 85, 168, 124, 173, 88, 142, 77, 171, 80, 178, 8, 218, 230, 68, 85, 231, 39, 54, 184, 42, 162, 85, 172, 139, 54, 173, 194, 7, 64, 250,
                32, 0, 0, 0, 0, 0, 0, 0,
                    115, 101, 101, 100, 115, 101, 101, 100, 115, 101, 101, 100, 115, 101, 101, 100, 115, 101, 101, 100, 115, 101, 101, 100, 115, 101, 101, 100, 115, 101, 101, 100,
                42, 0, 0, 0, 0, 0, 0, 0,
                200, 0, 0, 0, 0, 0, 0, 0,
                6, 161, 216, 23, 145, 55, 84, 42, 152, 52, 55, 189, 254, 42, 122, 178, 85, 127, 83, 92, 138, 120, 114, 43, 104, 164, 157, 192, 0, 0, 0, 0,
            /* initialize */
            7,
            2,
                3, 5,
            116,
                0, 0, 0, 0,
                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    MessageHeader header;
    Parser parser = { message, sizeof(message) };
    assert(parse_message_header(&parser, &header) == 0);
    transaction_summary_reset();
    assert(process_message_body(parser.buffer, parser.buffer_length, &header) == 0);
    transaction_summary_set_fee_payer_pubkey(&header.pubkeys[0]);

    enum SummaryItemKind kinds[MAX_TRANSACTION_SUMMARY_ITEMS];
    size_t num_kinds;
    assert(transaction_summary_finalize(kinds, &num_kinds) == 0);
    assert(num_kinds == 12);
    for (size_t i = 0; i < num_kinds; i++) {
        assert(transaction_summary_display_item(i) == 0);
    }
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
    test_process_message_body_nonced_stake_create_with_seed();

    printf("passed\n");
    return 0;
}
