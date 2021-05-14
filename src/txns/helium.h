#include <stdbool.h>
#include <stdint.h>
#include <os.h>
#include <cx.h>
#include "../ux/helium_ux.h"

// exception codes
#define SW_DEVELOPER_ERR 0x6B00
#define SW_INVALID_PARAM 0x6B01
#define SW_IMPROPER_INIT 0x6B02
#define SW_USER_REJECTED 0x6985
#define SW_OK            0x9000

// macros for converting raw bytes to uint64_t
#define U8BE(buf, off) (((uint64_t)(U4BE(buf, off))     << 32) | ((uint64_t)(U4BE(buf, off + 4)) & 0xFFFFFFFF))
#define U8LE(buf, off) (((uint64_t)(U4LE(buf, off + 4)) << 32) | ((uint64_t)(U4LE(buf, off))     & 0xFFFFFFFF))

// bin2hex converts binary to hex and appends a final NUL byte.
void bin2hex(uint8_t *dst, uint8_t *data, uint64_t inlen);

// bin2dec converts an unsigned integer to a decimal string and appends a
// final NUL byte. It returns the length of the string.
int bin2dec(uint8_t *dst, uint64_t n);

uint32_t pretty_print_hnt(uint8_t *dst, uint64_t n);

typedef struct transaction_arg_t {
    uint8_t * buf;
    uint16_t buf_len;
    uint64_t * amount;
    uint64_t * nonce;
    uint64_t * fee; 
    unsigned char * payee;
} transaction_arg_t;

extern bool sign_transaction;
extern uint16_t txn_length;

uint32_t create_helium_pay_txn(uint8_t account_index);
uint32_t create_helium_stake_txn(uint8_t account);
uint32_t create_helium_transfer_validator_txn(uint8_t account);
uint32_t create_helium_unstake_txn(uint8_t account);

#define SIZE_OF_PUB_KEY_BIN 	32
#define SIZE_OF_SHA_CHECKSUM 	4
#define SIZEOF_HELIUM_KEY	SIZE_OF_PUB_KEY_BIN + 1
#define SIZEOF_SIGNATURE	64

#define KEYTYPE_ED25519 0x01
#define NETTYPE_MAIN 0x00
#define NETTYPE_TEST 0x10

#define P1_PUBKEY_DISPLAY_ON	0x01
#define P1_PUBKEY_DISPLAY_OFF 	0x00

void get_pubkey_bytes(uint8_t account_index, uint8_t * out);
#define MAX_ENC_INPUT_SIZE 120

int btchip_encode_base58(const unsigned char *in, size_t length,
                         unsigned char *out, size_t *outlen);

// This symbol is defined by the link script to be at the start of the stack
// area.
extern unsigned long _stack;

#define STACK_CANARY (*((volatile uint32_t*) &_stack))

void init_canary();
void check_canary();
