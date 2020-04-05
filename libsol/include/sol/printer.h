#pragma once

#include <stdint.h>
#include <stddef.h>

#define SUMMARY_LENGTH 7
#define TITLE_SIZE 32
#define BASE58_PUBKEY_LENGTH 45
#define BASE58_PUBKEY_SHORT (SUMMARY_LENGTH + 2 + SUMMARY_LENGTH + 1)

typedef struct field_t {
    char title[TITLE_SIZE];
    char text[BASE58_PUBKEY_LENGTH];
} field_t;

int print_amount(uint64_t amount, const char *asset, char *out, size_t out_length);

int print_u64(uint64_t u64, char* out, size_t out_length);

int print_summary(const char *in, char *out, size_t out_length, size_t left_length, size_t right_length);

int encode_base58(const void *in, size_t length, char *out, size_t maxoutlen);
