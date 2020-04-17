#pragma once

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#define BAIL_IF(x) {int err = x; if (err) return err;}
#define MIN(a, b) ((a) < (b) ? (a) : (b));

#define assert_string_equal(actual, expected) \
    assert(strcmp(actual, expected) == 0)
