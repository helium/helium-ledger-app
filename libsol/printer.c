#include <string.h>
#include "printer.h"
#include "os_error.h"

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

static const char BASE58_ALPHABET[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
                                        'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
                                        'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                                        'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                        'w', 'x', 'y', 'z'};

int encode_base58(uint8_t *in, uint8_t length, uint8_t *out, uint8_t maxoutlen) {
    uint8_t tmp[164];
    uint8_t buffer[164];
    uint8_t j;
    uint8_t start_at;
    uint8_t zero_count = 0;
    if (length > sizeof(tmp)) {
        return INVALID_PARAMETER;
    }
    memmove(tmp, in, length);
    while ((zero_count < length) && (tmp[zero_count] == 0)) {
        ++zero_count;
    }
    j = 2 * length;
    start_at = zero_count;
    while (start_at < length) {
        uint16_t remainder = 0;
        uint8_t div_loop;
        for (div_loop = start_at; div_loop < length; div_loop++) {
            uint16_t digit256 = (uint16_t)(tmp[div_loop] & 0xff);
            uint16_t tmp_div = remainder * 256 + digit256;
            tmp[div_loop] = (uint8_t)(tmp_div / 58);
            remainder = (tmp_div % 58);
        }
        if (tmp[start_at] == 0) {
            ++start_at;
        }
        buffer[--j] = (uint8_t)BASE58_ALPHABET[remainder];
    }
    while ((j < (2 * length)) && (buffer[j] == BASE58_ALPHABET[0])) {
        ++j;
    }
    while (zero_count-- > 0) {
        buffer[--j] = BASE58_ALPHABET[0];
    }
    length = 2 * length - j;
    if (maxoutlen < length + 1) {
        return EXCEPTION_OVERFLOW;
    }
    memmove(out, (buffer + j), length);
    out[length] = '\0';
    return 0;
}
