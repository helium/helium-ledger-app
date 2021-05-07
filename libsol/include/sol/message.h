#pragma once

#include "parser.h"

int process_message_body(
    const uint8_t* message_body,
    int message_body_length,
    const MessageHeader* header
);
