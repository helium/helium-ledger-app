#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SIZEOF_B58_KEY 34

typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[55]; // variable length
    // partialStr contains 12 characters of a longer string. This allows text
    // to be scrolled.
    uint8_t partialStr[13];
    uint8_t fullStr_len;
} getPublicKeyContext_t;

typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[55]; // variable length
    // partialStr contains 12 characters of a longer string. This allows text
    // to be scrolled.
    uint8_t partialStr[13];
    uint8_t fullStr_len;
    uint8_t account_index;
    uint64_t amount;
    uint64_t nonce;
    uint64_t fee;
    unsigned char payee[34];
    uint64_t memo;
} paymentContext_t;

typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[55]; // variable length
    // partialStr contains 12 characters of a longer string. This allows text
    // to be scrolled.
    uint8_t partialStr[13];
    uint8_t fullStr_len;
    uint8_t account_index;
    uint64_t stake;
    uint64_t nonce;
    uint64_t fee;
    unsigned char address[SIZEOF_B58_KEY];
} stakeValidatorContext_t;

typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[55]; // variable length
    // partialStr contains 12 characters of a longer string. This allows text
    // to be scrolled.
    uint8_t partialStr[13];
    uint8_t fullStr_len;
    uint8_t account_index;
    uint64_t stake_amount;
    uint64_t stake_release_height;
    uint64_t nonce;
    uint64_t fee;
    unsigned char address[SIZEOF_B58_KEY];
} unstakeValidatorContext_t;

typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[55]; // variable length
    // partialStr contains 12 characters of a longer string. This allows text
    // to be scrolled.
    uint8_t partialStr[13];
    uint8_t fullStr_len;
    uint8_t account_index;
    uint64_t stake_amount;
    uint64_t payment_amount;
    uint64_t fee;
    unsigned char old_owner[SIZEOF_B58_KEY];
    unsigned char new_owner[SIZEOF_B58_KEY];
    unsigned char old_address[SIZEOF_B58_KEY];
    unsigned char new_address[SIZEOF_B58_KEY];
} transferValidatorContext_t;

typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[55]; // variable length
    // partialStr contains 12 characters of a longer string. This allows text
    // to be scrolled.
    uint8_t partialStr[13];
    uint8_t fullStr_len;
    uint8_t account_index;
    uint64_t amount;
    uint64_t nonce;
    uint64_t fee;
    uint64_t memo;
    unsigned char payee[34];
} burnContext_t;

typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[55]; // variable length
    // partialStr contains 12 characters of a longer string. This allows text
    // to be scrolled.
    uint8_t partialStr[13];
    uint8_t fullStr_len;
    uint8_t account_index;
    uint64_t amount;
    uint64_t nonce;
    uint64_t fee;
    unsigned char payee[34];
} transferSecContext_t;


void save_payment_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, paymentContext_t *ctx);
void save_stake_validator_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, stakeValidatorContext_t *ctx);
void save_transfer_validator_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferValidatorContext_t *ctx);
void save_unstake_validator_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, unstakeValidatorContext_t *ctx);
void save_burn_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, burnContext_t *ctx);
void save_transfer_sec_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferSecContext_t *ctx);

// Each command has some state associated with it that sticks around for the
// life of the command. A separate context_t struct should be defined for each
// command.
typedef union {
    getPublicKeyContext_t getPublicKeyContext;
    paymentContext_t paymentContext;
    stakeValidatorContext_t stakeValidatorContext;
    transferValidatorContext_t transferValidatorContext;
    unstakeValidatorContext_t unstakeValidatorContext;
    burnContext_t burnContext;
    transferSecContext_t transferSecContext;
} commandContext;

extern commandContext global;