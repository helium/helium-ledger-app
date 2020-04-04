#include "printer.c"
#include <assert.h>
#include <stdio.h>

#define assert_string_equal(actual, expected) assert(strcmp(actual, expected) == 0)

void test_print_amount() {
  char printed[24];
  const char *asset = "SOL";

  print_amount(1, asset, printed);
  assert_string_equal(printed, "0.000000001 SOL");
  print_amount(1000000000, asset, printed);
  assert_string_equal(printed, "1 SOL");
  print_amount(10000000000000001, asset, printed);
  assert_string_equal(printed, "10000000.000000001 SOL");
  print_amount(10000000001, asset, printed);
  assert_string_equal(printed, "10.000000001 SOL");
  print_amount(10000000100000000, asset, printed);
  assert_string_equal(printed, "10000000.1 SOL");
}

void test_print_summary() {
  char summary[27];
  print_summary("GADFVW3UXVKDOU626XUPYDJU2BFCGFJHQ6SREYOZ6IJV4XSHOALEQN2I",
                summary, 12, 12);
  assert_string_equal(summary, "GADFVW3UXVKD..4XSHOALEQN2I");
  print_summary("GADFVW3UXVKDOU626XUPYDJU2BFCGFJHQ6SREYOZ6IJV4XSHOALEQN2I",
                summary, 6, 6);
  assert_string_equal(summary, "GADFVW..LEQN2I");
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
    test_print_summary();
    test_print_u64();

    printf("passed\n");
    return 0;
}
