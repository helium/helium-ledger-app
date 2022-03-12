#pragma once

#include "parser.h"
#include <stdbool.h>

typedef struct PrintConfig {
    MessageHeader header;
    bool expert_mode;
} PrintConfig;
