#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "../../src/save_context.h"

static void test_save_context(void **state) {

}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_save_context)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
