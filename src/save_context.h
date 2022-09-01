#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nanos_paging.h"

#define SIZEOF_B58_KEY 34

#define TOKEN_TYPE_HNT 0
#define TOKEN_TYPE_HST 1
#define TOKEN_TYPE_MOB 2
#define TOKEN_TYPE_IOT 3
#define TOKEN_TYPE_MAX 3
typedef struct {
    uint8_t displayIndex;
    uint8_t fullStr[HELIUM_UX_MAX_CHARS+1];
    uint8_t partialStr[CHARS_PER_PAGE+1];
    uint8_t fullStr_len;
    uint8_t account_index;
    uint8_t title[HELIUM_UX_MAX_TITLE+1];
    uint8_t title_len;
    bool lock;
} globalContext_t;

typedef struct {
    uint64_t amount;
    uint64_t nonce;
    uint64_t fee;
    unsigned char payee[SIZEOF_B58_KEY];
    uint64_t memo;
    uint8_t token;
} paymentContext_t;

typedef struct {
    uint64_t stake;
    uint64_t nonce;
    uint64_t fee;
    unsigned char address[SIZEOF_B58_KEY];
} stakeValidatorContext_t;

typedef struct {
    uint64_t stake_amount;
    uint64_t stake_release_height;
    uint64_t nonce;
    uint64_t fee;
    unsigned char address[SIZEOF_B58_KEY];
} unstakeValidatorContext_t;

typedef struct {
    uint64_t stake_amount;
    uint64_t payment_amount;
    uint64_t fee;
    unsigned char old_owner[SIZEOF_B58_KEY];
    unsigned char new_owner[SIZEOF_B58_KEY];
    unsigned char old_address[SIZEOF_B58_KEY];
    unsigned char new_address[SIZEOF_B58_KEY];
} transferValidatorContext_t;

typedef struct {
    uint64_t amount;
    uint64_t nonce;
    uint64_t fee;
    uint64_t memo;
    unsigned char payee[SIZEOF_B58_KEY];
} burnContext_t;

typedef struct {
    uint64_t amount;
    uint64_t nonce;
    uint64_t fee;
    unsigned char payee[SIZEOF_B58_KEY];
} transferSecContext_t;


bool save_payment_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, paymentContext_t *ctx);
bool save_stake_validator_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, stakeValidatorContext_t *ctx);
bool save_transfer_validator_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferValidatorContext_t *ctx);
bool save_unstake_validator_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, unstakeValidatorContext_t *ctx);
bool save_burn_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, burnContext_t *ctx);
bool save_transfer_sec_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferSecContext_t *ctx);

// Each command has some state associated with it that sticks around for the
// life of the command. A separate context_t struct should be defined for each
// command.
typedef union {
    paymentContext_t paymentContext;
    stakeValidatorContext_t stakeValidatorContext;
    transferValidatorContext_t transferValidatorContext;
    unstakeValidatorContext_t unstakeValidatorContext;
    burnContext_t burnContext;
    transferSecContext_t transferSecContext;
} commandContext_t;

extern globalContext_t global;
extern commandContext_t cmd;