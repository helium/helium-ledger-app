#include <string.h>
#include "printer.h"

// max amount is max uint64 scaled down: "9223372036.854775807"
#define AMOUNT_MAX_SIZE 22

int print_amount(uint64_t amount, const char *asset, char *out) {
    char buffer[AMOUNT_MAX_SIZE];
    uint64_t dVal = amount;
    int i, j;

    memset(buffer, 0, AMOUNT_MAX_SIZE);
    for (i = 0; dVal > 0 || i < 11; i++) {
        if (dVal > 0) {
            buffer[i] = (dVal % 10) + '0';
            dVal /= 10;
        } else {
            buffer[i] = '0';
        }
        if (i == 8) { // lamports to SOL: 1 SOL = 1,000,000,000 lamports
            i += 1;
            buffer[i] = '.';
        }
        if (i >= AMOUNT_MAX_SIZE) {
            return 1;
        }
    }
    // Reverse order
    for (i -= 1, j = 0; i >= 0 && j < AMOUNT_MAX_SIZE-1; i--, j++) {
        out[j] = buffer[i];
    }
    // Strip trailing 0s
    for (j -= 1; j > 0; j--) {
        if (out[j] != '0') break;
    }
    j += 1;

    // Strip trailing .
    if (out[j-1] == '.') j -= 1;

    if (asset) {
        // Qualify amount
        out[j++] = ' ';
        strcpy(out + j, asset);
        out[j+strlen(asset)] = '\0';
    } else {
        out[j] = '\0';
    }

    return 0;
}

int print_summary(const char *in, char *out, uint8_t left_length, uint8_t right_length) {
    uint8_t out_length = left_length + right_length + 2;
    uint16_t in_length = strlen(in);
    if (in_length > out_length) {
        memcpy(out, in, left_length);
        out[left_length] = '.';
        out[left_length + 1] = '.';
        memcpy(out + left_length + 2, in + in_length - right_length, right_length);
        out[out_length] = '\0';
    } else {
        memcpy(out, in, in_length);
    }

    return 0;
}
