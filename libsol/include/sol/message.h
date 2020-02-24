#pragma once

int process_message_body(uint8_t* message_body, int message_body_length, MessageHeader* header, field_t* fields, size_t* fields_used);
