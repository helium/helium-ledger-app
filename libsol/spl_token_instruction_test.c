#include "common_byte_strings.h"
#include "instruction.h"
#include "spl_token_instruction.c"
#include <stdio.h>
#include <assert.h>

void print_pubkey(const Pubkey* pubkey) {
    char buf[45];
    encode_base58(pubkey, 32, buf, sizeof(buf));
    printf("%s\n", buf);
}

#define BLOCKHASH       BYTES32_BS58_1
#define MINT_ADDRESS    BYTES32_BS58_2
#define OWNER_PUBKEY    BYTES32_BS58_3
#define TOKEN_ACCOUNT   BYTES32_BS58_4
#define SIGNER1         BYTES32_BS58_5

void test_parse_spl_token_create_token() {
    uint8_t message[] = {
        0x02, 0x00, 0x02,
        0x04,
            OWNER_PUBKEY,
            MINT_ADDRESS,
            PROGRAM_ID_SYSTEM,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        0x02,
            // SystemCreateAccount
            0x02,
            0x02,
                0x00, 0x01,
            0x34,
                0x00, 0x00, 0x00, 0x00,
                0x80, 0xd7, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                PROGRAM_ID_SPL_TOKEN,
            // SplTokenInitializeMint
            0x03,
            0x02,
                0x01, 0x00,
            0x0a,
                0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x09,
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SystemCreateAccount (ignored)
    assert(instruction_validate(&instruction, &header) == 0);
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenInitializeMint
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(InitializeMint));

    const SplTokenInitializeMintInfo* init_mint = &info.initialize_mint;

    const Pubkey mint_account = {{ MINT_ADDRESS }};
    assert_pubkey_equal(init_mint->mint_account, &mint_account);

    assert(init_mint->token_account == NULL);

    const Pubkey owner = {{ OWNER_PUBKEY }};
    assert_pubkey_equal(init_mint->owner, &owner);

    assert(init_mint->body.amount == 0);
    assert(init_mint->body.decimals == 9);
}

void test_parse_spl_token_create_account() {
    uint8_t message[] = {
        0x02, 0x00, 0x03,
        0x05,
            OWNER_PUBKEY,
            TOKEN_ACCOUNT,
            MINT_ADDRESS,
            PROGRAM_ID_SYSTEM,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        0x02,
            // SystemCreateAccount
            0x03,
            0x02,
                0x00, 0x01,
            0x34,
                0x00, 0x00, 0x00, 0x00,
                0x80, 0x56, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                PROGRAM_ID_SPL_TOKEN,
            // SplTokenInitializeAccount
            0x04,
            0x03,
                0x01, 0x02, 0x00,
            0x01,
                0x01,
    };

    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SystemCreateAccount (ignored)
    assert(instruction_validate(&instruction, &header) == 0);
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenInitializeAccount
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(InitializeAccount));
    const SplTokenInitializeAccountInfo* init_acc = &info.initialize_account;

    const Pubkey token_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(init_acc->token_account, &token_account);

    const Pubkey mint_account = {{ MINT_ADDRESS }};
    assert_pubkey_equal(init_acc->mint_account, &mint_account);

    const Pubkey owner = {{ OWNER_PUBKEY }};
    assert_pubkey_equal(init_acc->owner, &owner);
}

void test_parse_spl_token_create_multisig() {
}

void test_parse_spl_token_transfer() {
    uint8_t message[] = {
        1, 0, 1,
        4,
            10, 197, 71, 166, 84, 143, 238, 106, 60, 71, 210, 140, 50, 46, 5, 64,
                197, 233, 184, 185, 240, 1, 189, 60, 85, 208, 255, 255, 23, 193, 128, 222,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            6, 221, 246, 225, 184, 247, 138, 134, 222, 155, 148, 231, 93, 80, 227, 74,
                129, 1, 199, 34, 198, 187, 150, 187, 221, 211, 20, 46, 142, 46, 111, 170,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1,
            3,
                3, 1, 2,
            0,
            9,
                3,
                42, 0, 0, 0, 0, 0, 0, 0
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenTransfer
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(Transfer));
}

void test_parse_spl_token_approve() {
}

void test_parse_spl_token_revoke() {
}

void test_parse_spl_token_set_owner() {
}

void test_parse_spl_token_mint_to() {
}

void test_parse_spl_token_burn() {
}

void test_parse_spl_token_close_account() {
    uint8_t message[] = {
        0x01, 0x00, 0x01,
        0x03,
            OWNER_PUBKEY,
            TOKEN_ACCOUNT,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        0x01,
            // SplTokenCloseAccount
            0x02,
            0x03,
                0x01, 0x00, 0x00,
            0x01,
                0x09,
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenCloseAccount
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(CloseAccount));
    const SplTokenCloseAccountInfo* close_acc = &info.close_account;

    const Pubkey token_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(close_acc->token_account, &token_account);

    const Pubkey owner = {{ OWNER_PUBKEY }};
    assert_pubkey_equal(close_acc->dest_account, &owner);

    assert_pubkey_equal(close_acc->sign.single.signer, &owner);
}

int main() {
    test_parse_spl_token_create_token();
    test_parse_spl_token_create_account();
    test_parse_spl_token_create_multisig();
    test_parse_spl_token_transfer();
    test_parse_spl_token_approve();
    test_parse_spl_token_revoke();
    test_parse_spl_token_set_owner();
    test_parse_spl_token_mint_to();
    test_parse_spl_token_burn();
    test_parse_spl_token_close_account();

    printf("passed\n");
    return 0;
}
