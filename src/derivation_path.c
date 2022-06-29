#include "derivation_path.h"
#include "globals.h"

size_t read_derivation_path(const uint8_t *dataBuffer, size_t size, uint32_t *derivationPath) {
    if (size == 0) {
        THROW(ApduReplySolanaInvalidMessage);
    }
    size_t len = dataBuffer[0];
    dataBuffer += 1;
    if (len < 0x01 || len > MAX_BIP32_PATH_LENGTH) {
        THROW(ApduReplySolanaInvalidMessage);
    }
    if (1 + 4 * len > size) {
        THROW(ApduReplySolanaInvalidMessage);
    }

    for (unsigned int i = 0; i < len; i++) {
        derivationPath[i] = ((dataBuffer[0] << 24u) | (dataBuffer[1] << 16u) |
                             (dataBuffer[2] << 8u) | (dataBuffer[3]));
        dataBuffer += 4;
    }
    return len;
}

