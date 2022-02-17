#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <cmocka.h>

#include "../../src/save_context.h"

static void test_save_context(void **state) {
    uint8_t payee[] = {0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42, };
    paymentContext_t ctx;
    uint8_t payment_buffer[] = {248, 191, 133, 0, 0, 0, 0, 0, 184, 136, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 149, 222, 195, 16, 5, 249, 3, 234, 179, 175, 194, 131, 71, 143, 176, 224, 107, 71, 55, 65, 95, 63, 131, 224, 66, 211, 117, 253, 250, 87, 190, 42, 210, 4, 0, 0, 0, 0, 0, 0, };
    save_payment_context(3, 0, payment_buffer, 66, &ctx);
    assert(ctx.amount == 8765432);
    assert(ctx.fee == 35000);
    assert(ctx.account_index == 3);
    assert(ctx.memo == 1234);
    for(uint8_t i=0; i<34; i++) {
        assert(ctx.payee[i] == payee[i]);
    }
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_save_context)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
