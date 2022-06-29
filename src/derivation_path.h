#pragma once
#include <stdint.h>
#include <stdlib.h>

#define MAX_BIP32_PATH_LENGTH 5

size_t read_derivation_path(
    const uint8_t *dataBuffer,
    size_t size,
    uint32_t *derivationPath
);
