#include "helium.h"

void save_payment_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, paymentContext_t *ctx) {
    ctx->amount = U8LE(dataBuffer, 0);
    ctx->fee = U8LE(dataBuffer, 8);
    ctx->nonce = U8LE(dataBuffer, 16);
    ctx->account_index = p1;
    os_memmove(ctx->payee, &dataBuffer[24], sizeof(ctx->payee));
    ctx->memo = U8LE(dataBuffer, 24+SIZEOF_B58_KEY);
}

void save_validator_stake_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, stakeValidatorContext_t *ctx) {
    ctx->stake_amount = U8LE(dataBuffer, 0);
    ctx->fee  = U8LE(dataBuffer, 8);
    ctx->account_index = p1;
    os_memmove(ctx->address, &dataBuffer[16], sizeof(ctx->address));
}

void save_validator_transfer_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferValidatorContext_t *ctx) {
    ctx->stake_amount = U8LE(dataBuffer, 0);
    ctx->payment_amount  = U8LE(dataBuffer, 8);
    ctx->fee = U8LE(dataBuffer, 16);
    ctx->account_index = p1;
    os_memmove(ctx->new_owner, &dataBuffer[24], sizeof(ctx->new_owner));
    os_memmove(ctx->old_owner, &dataBuffer[24+SIZEOF_B58_KEY], sizeof(ctx->old_owner));
    os_memmove(ctx->new_address, &dataBuffer[24+2*SIZEOF_B58_KEY], sizeof(ctx->new_address));
    os_memmove(ctx->old_address, &dataBuffer[24+3*SIZEOF_B58_KEY], sizeof(ctx->old_address));
}

void save_validator_unstake_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, unstakeValidatorContext_t *ctx) {
    ctx->stake_amount = U8LE(dataBuffer, 0);
    ctx->stake_release_height = U8LE(dataBuffer, 8);
    ctx->fee  = U8LE(dataBuffer, 16);
    ctx->account_index = p1;
    os_memmove(ctx->address, &dataBuffer[24], sizeof(ctx->address));
}

void save_burn_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, burnContext_t *ctx) {
    ctx->amount = U8LE(dataBuffer, 0);
    ctx->fee = U8LE(dataBuffer, 8);
    ctx->nonce = U8LE(dataBuffer, 16);
    ctx->memo = U8LE(dataBuffer, 24);
    ctx->account_index = p1;
    os_memmove(ctx->payee, &dataBuffer[32], sizeof(ctx->payee));
}


