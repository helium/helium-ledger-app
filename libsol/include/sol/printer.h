#pragma once

#include <stdint.h>
#include <stddef.h>

#define SUMMARY_LENGTH 7
#define TITLE_SIZE 32
#define BASE58_PUBKEY_LENGTH 45

typedef struct field_t {
    char title[TITLE_SIZE];
    char text[BASE58_PUBKEY_LENGTH];
} field_t;

int print_amount(uint64_t amount, const char *asset, char *out);

int print_summary(const char *in, char *out, uint8_t left_length, uint8_t right_length);

int encode_base58(uint8_t *in, uint8_t length, uint8_t *out, uint8_t maxoutlen);
