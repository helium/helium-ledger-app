#include <string.h>
#include <stdbool.h>
#include "save_context.h"

static inline uint32_t U4LE(const uint8_t *buf, size_t off) {
    return (((uint32_t)buf[off + 3]) << 24) | (buf[off + 2] << 16) |
                                              (buf[off + 1] << 8) | buf[off];
}
// macros for converting raw bytes to uint64_t
#define U8LE(buf, off) (((uint64_t)(U4LE(buf, off + 4)) << 32) | ((uint64_t)(U4LE(buf, off))     & 0xFFFFFFFF))

bool save_payment_context(uint8_t p1, __attribute__((unused)) uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, paymentContext_t *ctx) {
    global.lock = false;
    uint8_t min_data_len = 24 + SIZEOF_B58_KEY + 8;
    if (dataLength >= 24 + SIZEOF_B58_KEY + 8) {
        ctx->amount = U8LE(dataBuffer, 0);
        ctx->fee = U8LE(dataBuffer, 8);
        ctx->nonce = U8LE(dataBuffer, 16);
        global.account_index = p1;
        memmove(ctx->payee, &dataBuffer[24], sizeof(ctx->payee));
        ctx->memo = U8LE(dataBuffer, 24+SIZEOF_B58_KEY);
        if (dataLength >= min_data_len + 1) {
            ctx->token = dataBuffer[24 + SIZEOF_B58_KEY + 8];
            // if we have an invalid token_type, bail out
            if(ctx->token > TOKEN_TYPE_MAX) {
                return false;
            }
        } else {
            ctx->token = TOKEN_TYPE_HNT;
        }
        return true;
    } else {
        return false;
    }
}

bool save_stake_validator_context(uint8_t p1, __attribute__((unused)) uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, stakeValidatorContext_t *ctx) {
    global.lock = false;
    if (dataLength >= 16 + sizeof(ctx->address)) {
        ctx->stake = U8LE(dataBuffer, 0);
        ctx->fee  = U8LE(dataBuffer, 8);
        global.account_index = p1;
        memmove(ctx->address, &dataBuffer[16], sizeof(ctx->address));
        return true;
    } else {
        return false;
    }
}

bool save_transfer_validator_context(uint8_t p1, __attribute__((unused)) uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferValidatorContext_t *ctx) {
    global.lock = false;
    if (dataLength >= 24+3*SIZEOF_B58_KEY + sizeof(ctx->old_address)) {
        ctx->stake_amount = U8LE(dataBuffer, 0);
        ctx->payment_amount  = U8LE(dataBuffer, 8);
        ctx->fee = U8LE(dataBuffer, 16);
        global.account_index = p1;
        memmove(ctx->new_owner, &dataBuffer[24], sizeof(ctx->new_owner));
        memmove(ctx->old_owner, &dataBuffer[24+SIZEOF_B58_KEY], sizeof(ctx->old_owner));
        memmove(ctx->new_address, &dataBuffer[24+2*SIZEOF_B58_KEY], sizeof(ctx->new_address));
        memmove(ctx->old_address, &dataBuffer[24+3*SIZEOF_B58_KEY], sizeof(ctx->old_address));
        return true;
    } else {
        return false;
    }
}

bool save_unstake_validator_context(uint8_t p1, __attribute__((unused)) uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, unstakeValidatorContext_t *ctx) {
    global.lock = false;
    if (dataLength >= 24 + sizeof(ctx->address)) {
        ctx->stake_amount = U8LE(dataBuffer, 0);
        ctx->stake_release_height = U8LE(dataBuffer, 8);
        ctx->fee  = U8LE(dataBuffer, 16);
        global.account_index = p1;
        memmove(ctx->address, &dataBuffer[24], sizeof(ctx->address));
        return true;
    } else {
        return false;
    }
}

bool save_burn_context(uint8_t p1, __attribute__((unused)) uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, burnContext_t *ctx) {
    global.lock = false;
    if (dataLength >= 32 + sizeof(ctx->payee)) {
        ctx->amount = U8LE(dataBuffer, 0);
        ctx->fee = U8LE(dataBuffer, 8);
        ctx->nonce = U8LE(dataBuffer, 16);
        ctx->memo = U8LE(dataBuffer, 24);
        global.account_index = p1;
        memmove(ctx->payee, &dataBuffer[32], sizeof(ctx->payee));
        return true;
    } else {
        return false;
    }
}

bool save_transfer_sec_context(uint8_t p1, __attribute__((unused)) uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferSecContext_t *ctx) {
    global.lock = false;
    if (dataLength >= 24 + sizeof(ctx->payee)) {
        ctx->amount = U8LE(dataBuffer, 0);
        ctx->fee = U8LE(dataBuffer, 8);
        ctx->nonce = U8LE(dataBuffer, 16);
        global.account_index = p1;
        memmove(ctx->payee, &dataBuffer[24], sizeof(ctx->payee));
        return true;
    } else {
        return false;
    }
}



