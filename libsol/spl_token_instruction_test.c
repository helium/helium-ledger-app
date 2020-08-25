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
#define MINT_ACCOUNT    BYTES32_BS58_2
#define TOKEN_ACCOUNT   BYTES32_BS58_3
#define OWNER_ACCOUNT   BYTES32_BS58_4
#define MULTISIG_ACCOUNT    OWNER_ACCOUNT
#define SIGNER1         BYTES32_BS58_5
#define SIGNER2         BYTES32_BS58_6
#define SIGNER3         BYTES32_BS58_7
#define DEST_ACCOUNT    BYTES32_BS58_8
#define DELEGATE        DEST_ACCOUNT
#define NEW_OWNER       DEST_ACCOUNT

void test_parse_spl_token_create_token() {
    uint8_t message[] = {
        0x02, 0x00, 0x02,
        0x04,
            OWNER_ACCOUNT,
            MINT_ACCOUNT,
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

    const Pubkey mint_account = {{ MINT_ACCOUNT }};
    assert_pubkey_equal(init_mint->mint_account, &mint_account);

    assert(init_mint->token_account == NULL);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(init_mint->owner, &owner);

    assert(init_mint->body.amount == 0);
    assert(init_mint->body.decimals == 9);
}

void test_parse_spl_token_create_account() {
    uint8_t message[] = {
        0x02, 0x00, 0x03,
        0x05,
            OWNER_ACCOUNT,
            TOKEN_ACCOUNT,
            MINT_ACCOUNT,
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

    const Pubkey mint_account = {{ MINT_ACCOUNT }};
    assert_pubkey_equal(init_acc->mint_account, &mint_account);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(init_acc->owner, &owner);
}

void test_parse_spl_token_create_multisig() {
    uint8_t message[] = {
        2, 0, 5,
        7,
            OWNER_ACCOUNT,
            MULTISIG_ACCOUNT,
            SIGNER1,
            SIGNER2,
            SIGNER3,
            PROGRAM_ID_SYSTEM,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        2,
            // SystemCreateAccount
            5,
            2,
                0, 1,
            52,
                0, 0, 0, 0,
                245, 1, 0, 0, 0, 0, 0, 0,
                40, 0, 0, 0, 0, 0, 0, 0,
                PROGRAM_ID_SPL_TOKEN,
            // SplTokenInitializeMultisig
            6,
            4,
                1, 2, 3, 4,
            2,
                2,
                2
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SystemCreateAccount (ignored)
    assert(instruction_validate(&instruction, &header) == 0);
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenInitializeMultisig
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(InitializeMultisig));
    const SplTokenInitializeMultisigInfo* init_ms = &info.initialize_multisig;

    assert(init_ms->body.m == 2);

    const Pubkey multisig_account = {{ MULTISIG_ACCOUNT }};
    assert_pubkey_equal(init_ms->multisig_account, &multisig_account);

    const Pubkey* signer = init_ms->signers.first;
    const Pubkey signer1 = {{ SIGNER1 }};
    assert_pubkey_equal(signer++, &signer1);
    const Pubkey signer2 = {{ SIGNER2 }};
    assert_pubkey_equal(signer++, &signer2);
    const Pubkey signer3 = {{ SIGNER3 }};
    assert_pubkey_equal(signer++, &signer3);
}

void test_parse_spl_token_transfer() {
    uint8_t message[] = {
        1, 0, 1,
        4,
            OWNER_ACCOUNT,
            TOKEN_ACCOUNT,
            DEST_ACCOUNT,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        1,
            3,
            3,
                1, 2, 0,
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
    const SplTokenTransferInfo* tr_info = &info.transfer;

    assert(tr_info->body.amount == 42);

    const Pubkey src_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(tr_info->src_account, &src_account);

    const Pubkey dest_account = {{ DEST_ACCOUNT }};
    assert_pubkey_equal(tr_info->dest_account, &dest_account);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(tr_info->sign.single.signer, &owner);
}

void test_parse_spl_token_approve() {
    uint8_t message[] = {
        1, 0, 2,
        4,
            OWNER_ACCOUNT,
            TOKEN_ACCOUNT,
            DELEGATE,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        1,
            3,
            3,
                1, 2, 0,
            9,
                4,
                42, 0, 0, 0, 0, 0, 0, 0
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenApprove
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(Approve));
    const SplTokenApproveInfo* ap_info = &info.approve;

    assert(ap_info->body.amount == 42);

    const Pubkey token_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(ap_info->token_account, &token_account);

    const Pubkey delegate = {{ DELEGATE }};
    assert_pubkey_equal(ap_info->delegate, &delegate);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(ap_info->sign.single.signer, &owner);
}

void test_parse_spl_token_revoke() {
    uint8_t message[] = {
        1, 0, 2,
        3,
            OWNER_ACCOUNT,
            TOKEN_ACCOUNT,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        1,
            2,
            2,
                1, 0,
            1,
                5
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenRevoke
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(Revoke));
    const SplTokenRevokeInfo* re_info = &info.revoke;

    const Pubkey token_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(re_info->token_account, &token_account);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(re_info->sign.single.signer, &owner);
}

void test_parse_spl_token_set_owner() {
    uint8_t message[] = {
        1, 0, 2,
        4,
            OWNER_ACCOUNT,
            TOKEN_ACCOUNT,
            NEW_OWNER,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        1,
            3,
            3,
                1, 2, 0,
            1,
                6
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenSetOwner
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(SetOwner));
    const SplTokenSetOwnerInfo* so_info = &info.set_owner;

    const Pubkey token_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(so_info->token_account, &token_account);

    const Pubkey new_owner = {{ NEW_OWNER }};
    assert_pubkey_equal(so_info->new_owner, &new_owner);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(so_info->sign.single.signer, &owner);
}

void test_parse_spl_token_mint_to() {
    uint8_t message[] = {
        1, 0, 1,
        4,
            OWNER_ACCOUNT,
            MINT_ACCOUNT,
            TOKEN_ACCOUNT,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        1,
            3,
            3,
                1, 2, 0,
            9,
                7,
                42, 0, 0, 0, 0, 0, 0, 0
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenMintTo
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(MintTo));
    const SplTokenMintToInfo* mt_info = &info.mint_to;

    assert(mt_info->body.amount == 42);

    const Pubkey mint_account = {{ MINT_ACCOUNT }};
    assert_pubkey_equal(mt_info->mint_account, &mint_account);

    const Pubkey token_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(mt_info->token_account, &token_account);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(mt_info->sign.single.signer, &owner);
}

void test_parse_spl_token_burn() {
    uint8_t message[] = {
        1, 0, 1,
        3,
            OWNER_ACCOUNT,
            TOKEN_ACCOUNT,
            PROGRAM_ID_SPL_TOKEN,
        BLOCKHASH,
        1,
            2,
            2,
                1, 0,
            9,
                8,
                42, 0, 0, 0, 0, 0, 0, 0
    };
    Parser parser = {message, sizeof(message)};
    MessageHeader header;
    assert(parse_message_header(&parser, &header) == 0);

    Instruction instruction;
    assert(parse_instruction(&parser, &instruction) == 0); // SplTokenBurn
    assert(instruction_validate(&instruction, &header) == 0);

    SplTokenInfo info;
    assert(parse_spl_token_instructions(&instruction, &header, &info) == 0);
    assert(parser.buffer_length == 0);

    assert(info.kind == SplTokenKind(Burn));
    const SplTokenBurnInfo* bn_info = &info.burn;

    assert(bn_info->body.amount == 42);

    const Pubkey token_account = {{ TOKEN_ACCOUNT }};
    assert_pubkey_equal(bn_info->token_account, &token_account);

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(bn_info->sign.single.signer, &owner);
}

void test_parse_spl_token_close_account() {
    uint8_t message[] = {
        0x01, 0x00, 0x01,
        0x03,
            OWNER_ACCOUNT,
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

    const Pubkey owner = {{ OWNER_ACCOUNT }};
    assert_pubkey_equal(close_acc->dest_account, &owner);

    assert_pubkey_equal(close_acc->sign.single.signer, &owner);
}

void test_parse_spl_token_instruction_kind() {
    SplTokenInstructionKind kind;

    uint8_t buf[] = { 0 };
    Parser parser = { buf, ARRAY_LEN(buf) };
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(InitializeMint));

    buf[0] = 1;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(InitializeAccount));

    buf[0] = 2;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(InitializeMultisig));

    buf[0] = 3;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(Transfer));

    buf[0] = 4;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(Approve));

    buf[0] = 5;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(Revoke));

    buf[0] = 6;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(SetOwner));

    buf[0] = 7;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(MintTo));

    buf[0] = 8;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(Burn));

    buf[0] = 9;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 0);
    assert(kind == SplTokenKind(CloseAccount));

    // First unused enum value fails
    buf[0] = 10;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 1);

    // Largest buffer value fails
    buf[0] = 255;
    parser.buffer = buf;
    parser.buffer_length = ARRAY_LEN(buf);
    assert(parse_spl_token_instruction_kind(&parser, &kind) == 1);
}

void test_parse_spl_token_sign() {
    const size_t max_signers = Token_MAX_SIGNERS;
    const size_t max_accounts = 1 + max_signers; // multisig_account + signers
    const size_t accounts_len = max_accounts + 1; // one too many
    const Pubkey pubkeys[1];
    const MessageHeader header = {
        .pubkeys_header = { 0, 0, 0, 1},
        .pubkeys = pubkeys,
    };
    uint8_t accounts[accounts_len];
    memset(accounts, 0, accounts_len);
    Instruction ix = {
        .program_id_index = 0,
        .accounts = accounts,
        .accounts_length = 0,
    };
    InstructionAccountsIterator it;
    SplTokenSign sign;

    // too few remaining keys fails
    instruction_accounts_iterator_init(&it, &header, &ix);
    assert(parse_spl_token_sign(&it, &sign) == 1);

    // too many remaining keys fails
    ix.accounts_length = accounts_len;
    instruction_accounts_iterator_init(&it, &header, &ix);
    assert(parse_spl_token_sign(&it, &sign) == 1);

    // one key resolves to single signer
    ix.accounts_length = 1;
    instruction_accounts_iterator_init(&it, &header, &ix);
    assert(parse_spl_token_sign(&it, &sign) == 0);
    assert(sign.kind == SplTokenSignKindSingle);

    // two keys resolves to multi signer
    ix.accounts_length = 2;
    instruction_accounts_iterator_init(&it, &header, &ix);
    assert(parse_spl_token_sign(&it, &sign) == 0);
    assert(sign.kind == SplTokenSignKindMulti);
    assert(sign.multi.signers.count == 1);

    // max keys resolves to multi signer
    ix.accounts_length = max_accounts;
    instruction_accounts_iterator_init(&it, &header, &ix);
    assert(parse_spl_token_sign(&it, &sign) == 0);
    assert(sign.kind == SplTokenSignKindMulti);
    assert(sign.multi.signers.count == max_signers);
}

void test_print_m_of_n_string() {
    char s[M_OF_N_MAX_LEN];

    // n too big fails
    assert(print_m_of_n_string(0, Token_MAX_SIGNERS + 1, s, sizeof(s)) == 1);

    // m > n fails
    assert(print_m_of_n_string(1, 0, s, sizeof(s)) == 1);

    // Buffer too small fails
    assert(print_m_of_n_string(0, 0, s, sizeof(s) - 1) == 1);

    assert(print_m_of_n_string(3, 5, s, sizeof(s)) == 0);
    assert_string_equal(s, "3 of 5");
    assert(print_m_of_n_string(3, Token_MAX_SIGNERS, s, sizeof(s)) == 0);
    assert_string_equal(s, "3 of 11");
    assert(print_m_of_n_string(Token_MAX_SIGNERS, Token_MAX_SIGNERS, s, sizeof(s)) == 0);
    assert_string_equal(s, "11 of 11");
}

int main() {
    test_print_m_of_n_string();
    test_parse_spl_token_sign();
    test_parse_spl_token_instruction_kind();
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
