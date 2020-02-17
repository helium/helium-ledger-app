#pragma once

#include <stdint.h>
#include <stddef.h>

int print_amount(uint64_t amount, const char *asset, char *out);

int print_summary(const char *in, char *out, uint8_t left_length, uint8_t right_length);
