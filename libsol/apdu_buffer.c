

enum ApduBufferState {
  ApduBufferStateUninitialized = 0,
  ApduBufferStateIntialized,
  ApduBufferStatePayloadInProgress,
  ApduBufferStatePayloadComplete,
  ApduBufferStateError,
};

struct apdu_message_handler {
  uint8_t* command_buffer;
  size_t command_buffer_size;
  size_t command_buffer_write_offset;
  enum ApduBufferState state;
};

#define APDU_BUFFER_HANDLER_STATIC_INITIALIZER = {  \
  .command_buffer = NULL,                           \
  .command_buffer_size = 0,                         \
  .command_buffer_write_offset = 0,                 \
  .state = ApduBufferStateUninitialized,            \
}

void apdu_message_handler_initialize(struct apdu_message_handler* abh, uint8_t* command_buffer, size_t command_buffer_size) {
  assert(abh->state == ApduBufferStateUninitialized);

  abh->command_buffer = command_buffer;
  abh->command_buffer_size = command_buffer_size;
  abh->command_buffer_write_offset = 0;
  abh->state = ApduBufferStateInitialized;
}

#define CLA 0xE0

// header offsets
#define OFFSET_CLA 0
#define OFFSET_INS 1
#define OFFSET_P1 2
#define OFFSET_P2 3
#define OFFSET_LC 4
#define OFFSET_CDATA 5
#define DEPRECATED_OFFSET_CDATA 6

// instructions
#define DEPRECATED_INS_GET_APP_CONFIGURATION 1
#define DEPRECATED_INS_GET_PUBKEY 2
#define DEPRECATED_INS_SIGN_MESSAGE 3
#define INS_GET_APP_CONFIGURATION 4
#define INS_GET_PUBKEY 5
#define INS_SIGN_MESSAGE 6

struct apdu_header {
  uint8_t class;
  uint8_t instruction;
  uint8_t p1;
  uint8_t p2;
  size_t data_len;
  uint8_t* data;
  bool deprecated_host;
}

enum ApduHandlerResult {
  ApduHandlerResultOk = 0,
  ApduHandlerResultErrorTooSmall,
  ApduHandlerResultErrorTooLarge,
  ApduHandlerResultErrorInvalidClass,
  ApduHandlerResultErrorCorruptHeader,
}

int apdu_message_handler_handle_message(struct apdu_message_handler* abh, uint8_t* apdu_message, size_t apdu_message_len) {
  struct apdu_header header = {};
  // must at least hold the class and instruction
  if (!(apdu_message_len > OFFSET_INS)) return ApduHandlerResultErrorTooSmall;

  header.class = apdu_message[OFFSET_CLA];
  if (header.class != CLA) return ApduHandlerResultErrorInvalidClass;

  header.instruction = apdu_message[OFFSET_INS];
  switch (header.instruction) {
    case DEPRECATED_INS_GET_APP_CONFIGURATION:
    case DEPRECATED_INS_GET_PUBKEY:
    case DEPRECATED_INS_SIGN_MESSAGE:
    {
      // must at least hold a full deprecated header
      if (!(apdu_message_len >= DEPRECATED_OFFSET_CDATA)) return ApduHandlerResultErrorTooSmall;
      // deprecated data may be up to 64KiB
      if (!(apdu_message_len <= UINT16_MAX)) return ApduHandlerResultErrorTooLarge;

      size_t data_len = U2BE(apdu_message_handler, OFFSET_LC);

      if (!(rx == (data_len + DEPRECATED_OFFSET_CDATA))) return ApduHandlerResultErrorCorruptHeader;

      header.data_len = data_len;
      if (header.data_len > 0) {
        header.data = &apdu_message[DEPRECATED_OFFSET_CDATA];
      } else {
        header.data = NULL;
      }

      header.deprecated_host = true;

      break;
    }
    case INS_GET_APP_CONFIGURATION:
    case INS_GET_PUBKEY:
    case INS_SIGN_MESSAGE:
    {
      // must at least hold a full modern header
      if (!(apdu_message_len >= OFFSET_CDATA)) return ApduHandlerResultErrorTooSmall;
      // modern data may be up to 255B
      if (!(apdu_message_len <= UINT8_MAX)) return ApduHandlerResultErrorTooLarge;

      size_t data_len = apdu_message[OFFSET_LC];

      if (!(rx == (data_len + OFFSET_CDATA))) return ApduHandlerResultErrorCorruptHeader;

      header.data_len = data_len;
      if (header.data_len > 0) {
        header.data = &apdu_message[OFFSET_CDATA];
      } else {
        header.data = NULL;
      }

      header.deprecated_host = false;

      break;
    }
  }

  header.p1 = apdu_message[OFFSET_P1];
  header.p2 = apdu_message[OFFSET_P2];

  // deprecated signmessage had a u16 data length prefix... deal with that
  size_t data_len;
  if (header.deprecated_host && header.instruction == DEPRECATED_INS_SIGN_MESSAGE) {
    size_t len_len = sizeof(uint16_t);
    if (header.data_len < len_len) return ApduHandlerResultErrorTooSmall;
    data_len = U2BE(header.data, 0);
    header.data += len_len;
    header.data_len -= len_len;
    if (header.data_len != data_len) return ApduHandlerResultErrorInvalidLegacyLength;
  } else {
    data_len = header.data_len;
  }

  memcpy(abh->command_buffer + abh->command_buffer_write_offset, header.data + data_len);
  abh->command_buffer_write_offset += data_len;
}
