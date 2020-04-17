#include "printer.c"
#include <assert.h>
#include <stdio.h>

void test_print_amount() {
  char printed[24];
  const char *asset = "SOL";

  print_amount(1, asset, printed, sizeof(printed));
  assert_string_equal(printed, "0.000000001 SOL");
  print_amount(1000000000, asset, printed, sizeof(printed));
  assert_string_equal(printed, "1 SOL");
  print_amount(10000000000000001, asset, printed, sizeof(printed));
  assert_string_equal(printed, "10000000.000000001 SOL");
  print_amount(10000000001, asset, printed, sizeof(printed));
  assert_string_equal(printed, "10.000000001 SOL");
  print_amount(10000000100000000, asset, printed, sizeof(printed));
  assert_string_equal(printed, "10000000.1 SOL");
}

void test_print_sized_string() {
    char buf[5];
    const char test[] = { 0x74, 0x65, 0x73, 0x74 };
    SizedString string = { sizeof(test), test };

    assert(print_sized_string(&string, buf, sizeof(buf)) == 0);
    assert_string_equal(buf, "test");

    assert(print_sized_string(&string, buf, 4) == 1);
    assert_string_equal(buf, "te~");

    assert(print_sized_string(&string, buf, 2) == 1);
    assert_string_equal(buf, "~");

    assert(print_sized_string(&string, buf, 1) == 1);
    assert_string_equal(buf, "");
}

void test_print_string() {
    char buf[5];

    assert(print_string("fits", buf, sizeof(buf)) == 0);
    assert_string_equal("fits", buf);
    assert(print_string("too long", buf, sizeof(buf)) == 1);
    assert_string_equal("too~", buf);
    assert(print_string("too_long", buf, 2) == 1);
    assert_string_equal("~", buf);
    assert(print_string("too_long", buf, 1) == 1);
    assert_string_equal("", buf);
}

void test_print_summary() {
    char summary[27];
    assert(
        print_summary(
            "GADFVW3UXVKDOU626XUPYDJU2BFCGFJHQ6SREYOZ6IJV4XSHOALEQN2I",
            summary,
            sizeof(summary),
            12,
            12
        ) == 0
    );
    assert_string_equal(summary, "GADFVW3UXVKD..4XSHOALEQN2I");
    assert(
        print_summary(
            "GADFVW3UXVKDOU626XUPYDJU2BFCGFJHQ6SREYOZ6IJV4XSHOALEQN2I",
            summary,
            sizeof(summary),
            6,
            6
        ) == 0
    );
    assert_string_equal(summary, "GADFVW..LEQN2I");

    const char* test_fits = "short enough";
    assert(print_summary(test_fits, summary, sizeof(summary), 12, 12) == 0);
    assert_string_equal(summary, test_fits);

    assert(print_summary("buffer too small", NULL, 0, 12, 12) == 1);
}

void test_print_i64() {
    char buf[21];
    assert(print_i64(INT64_MIN, buf, sizeof(buf)) == 0);
    assert_string_equal(buf, "-9223372036854775808");
    assert(print_i64(INT64_MAX, buf, sizeof(buf)) == 0);
    assert_string_equal(buf, "9223372036854775807");
    assert(print_i64(0, buf, sizeof(buf)) == 0);
    assert_string_equal(buf, "0");
    assert(print_i64(-1, buf, sizeof(buf)) == 0);
    assert_string_equal(buf, "-1");
}

void test_print_u64() {
#define U64_MAX_STR (20 + 1) // strlen("18446744073709551615") + NUL
    char out[U64_MAX_STR];

    assert(print_u64(0, out, sizeof(out)) == 0);
    assert_string_equal(out, "0");
    assert(print_u64(UINT64_MAX, out, sizeof(out)) == 0);
    assert_string_equal(out, "18446744073709551615");

    assert(print_u64(UINT64_MAX, out, sizeof(out) - 1) == 1);
    assert(print_u64(0, NULL, 0) == 1);
}

int main() {
    test_print_amount();
    test_print_sized_string();
    test_print_string();
    test_print_summary();
    test_print_i64();
    test_print_u64();

    printf("passed\n");
    return 0;
}
