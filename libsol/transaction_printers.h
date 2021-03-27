#pragma once

int print_transaction(
    const MessageHeader* header,
    InstructionInfo* const * infos,
    size_t infos_length
);
