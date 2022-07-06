#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <cmocka.h>

#include "../../src/save_context.h"

static void test_save_payment_context(void **state) {
    uint8_t payee[] = {0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65,
                       95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    paymentContext_t ctx;
    uint8_t payment_buffer[] = {248, 191, 133, 0, 0, 0, 0, 0, 184, 136, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55,
                                65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42, 210, 4, 0, 0, 0, 0, 0, 0,};
    save_payment_context(3, 0, payment_buffer, 66, &ctx);
    assert(ctx.amount == 8765432);
    assert(ctx.fee == 35000);
    assert(global.account_index == 3);
    assert(ctx.memo == 1234);
    for (uint8_t i = 0; i < 34; i++) {
        assert(ctx.payee[i] == payee[i]);
    }
}

static void test_save_burn_context(void **state) {
    uint8_t payee[] = {0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65,
                       95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    burnContext_t ctx;
    uint8_t burn_buffer[] = {161, 123, 152, 0, 0, 0, 0, 0, 171, 57, 5, 0, 0, 0, 0, 0, 239, 82, 0, 0, 0, 0, 0, 0, 178,
                             51, 0, 0, 0, 0, 0, 0, 0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143,
                             176, 224, 107, 71, 55, 65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    save_burn_context(8, 0, burn_buffer, 66, &ctx);
    assert(ctx.amount == 9993121);
    assert(ctx.fee == 342443);
    assert(global.account_index == 8);
    assert(ctx.memo == 13234);
    for (uint8_t i = 0; i < 34; i++) {
        assert(ctx.payee[i] == payee[i]);
    }
}

static void test_save_sec_transfer_context(void **state) {
    uint8_t payee[] = {0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65,
                       95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    transferSecContext_t ctx;
    uint8_t transfer_sec_buffer[] = {3, 36, 0, 0, 0, 0, 0, 0, 87, 64, 52, 0, 0, 0, 0, 0, 34, 80, 20, 0, 0, 0, 0, 0, 0,
                                     1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107,
                                     71, 55, 65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    save_transfer_sec_context(8, 0, transfer_sec_buffer, 58, &ctx);
    assert(ctx.amount == 9219);
    assert(ctx.fee == 3424343);
    assert(global.account_index == 8);
    for (uint8_t i = 0; i < 34; i++) {
        assert(ctx.payee[i] == payee[i]);
    }
}

static void test_save_validator_stake_context(void **state) {
    uint8_t address[] = {0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55,
                         65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    stakeValidatorContext_t ctx;
    uint8_t stake_validator_buffer[] = {171, 49, 139, 0, 0, 0, 0, 0, 226, 43, 0, 0, 0, 0, 0, 0, 0, 1, 149, 222, 195, 16,
                                        5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65, 95, 63,
                                        131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    save_stake_validator_context(8, 0, stake_validator_buffer, 50, &ctx);
    assert(ctx.stake == 9122219);
    assert(ctx.fee == 11234);
    assert(global.account_index == 8);
    for (uint8_t i = 0; i < 34; i++) {
        assert(ctx.address[i] == address[i]);
    }
}
static void test_save_validator_transfer_context(void **state) {
    uint8_t old_owner[] = {0, 1, 254, 27, 239, 19, 31, 111, 227, 89, 175, 198, 118, 179, 184, 136, 217, 195, 157, 37, 83, 235, 201, 86, 218, 146, 24, 182, 20, 253, 168, 242, 79, 22, };
    uint8_t new_owner[] = {0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42, };
    uint8_t old_address[] = {0, 0, 118, 79, 164, 62, 124, 6, 162, 248, 5, 252, 133, 12, 43, 119, 134, 226, 11, 45, 248, 73, 124, 42, 197, 170, 150, 60, 233, 198, 218, 88, 216, 22, };
    uint8_t new_address[] = {0, 0, 72, 165, 207, 197, 7, 238, 199, 138, 210, 43, 135, 245, 56, 152, 44, 154, 19, 161, 48, 4, 168, 163, 111, 255, 6, 138, 37, 254, 138, 197, 156, 145, };
    transferValidatorContext_t ctx;
    uint8_t transfer_validator_buffer[] = {171, 49, 139, 0, 0, 0, 0, 0, 226, 43, 0, 0, 0, 0, 0, 0, 147, 53, 139, 0, 0, 0, 0, 0, 0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42, 0, 1, 254, 27, 239, 19, 31, 111, 227, 89, 175, 198, 118, 179, 184, 136, 217, 195, 157, 37, 83, 235, 201, 86, 218, 146, 24, 182, 20, 253, 168, 242, 79, 22, 0, 0, 72, 165, 207, 197, 7, 238, 199, 138, 210, 43, 135, 245, 56, 152, 44, 154, 19, 161, 48, 4, 168, 163, 111, 255, 6, 138, 37, 254, 138, 197, 156, 145, 0, 0, 118, 79, 164, 62, 124, 6, 162, 248, 5, 252, 133, 12, 43, 119, 134, 226, 11, 45, 248, 73, 124, 42, 197, 170, 150, 60, 233, 198, 218, 88, 216, 22, };
    save_transfer_validator_context(8, 0, transfer_validator_buffer, 160, &ctx);
    assert(ctx.stake_amount == 9122219);
    assert(ctx.payment_amount == 11234);
    assert(ctx.fee == 9123219);
    assert(global.account_index == 8);
    for(uint8_t i=0; i<34; i++) {
        assert(ctx.new_owner[i] == new_owner[i]);
    }
    for(uint8_t i=0; i<34; i++) {
        assert(ctx.old_owner[i] == old_owner[i]);
    }
    for(uint8_t i=0; i<34; i++) {
        assert(ctx.new_address[i] == new_address[i]);
    }
    for(uint8_t i=0; i<34; i++) {
        assert(ctx.old_address[i] == old_address[i]);
    }
}

static void test_save_validator_unstake_context(void **state) {
    uint8_t address[] = {0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55,
                         65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42,};
    unstakeValidatorContext_t ctx;
    uint8_t unstake_validator_buffer[] = {123, 194, 93, 54, 0, 0, 0, 0, 91, 133, 153, 30, 2, 0, 0, 0, 162, 196, 11, 0,
                                          0, 0, 0, 0, 0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71,
                                          143, 176, 224, 107, 71, 55, 65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87,
                                          190, 42,};
    save_unstake_validator_context(8, 0, unstake_validator_buffer, 58, &ctx);
    assert(ctx.stake_amount == 912114299);
    assert(ctx.stake_release_height == 9103312219);
    assert(ctx.fee == 771234);
    assert(global.account_index == 8);
    for (uint8_t i = 0; i < 34; i++) {
        assert(ctx.address[i] == address[i]);
    }
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_save_payment_context),
            cmocka_unit_test(test_save_burn_context),
            cmocka_unit_test(test_save_validator_stake_context),
            cmocka_unit_test(test_save_validator_transfer_context),
            cmocka_unit_test(test_save_validator_unstake_context),
            cmocka_unit_test(test_save_sec_transfer_context)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
