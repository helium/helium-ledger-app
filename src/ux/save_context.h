#include "helium_ux.h"

void save_payment_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, paymentContext_t *ctx) __attribute__((always_inline));
void save_validator_stake_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, stakeValidatorContext_t *ctx) __attribute__((always_inline));
void save_validator_transfer_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferValidatorContext_t *ctx) __attribute__((always_inline));
void save_validator_unstake_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, unstakeValidatorContext_t *ctx) __attribute__((always_inline));
void save_burn_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, burnContext_t *ctx);
void save_transfer_sec_context(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, transferSec_t *ctx);
