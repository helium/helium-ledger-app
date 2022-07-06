#include "bolos_target.h"

#ifdef HAVE_UX_FLOW

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include <cx.h>
#include "helium.h"
#include "helium_ux.h"
#include "save_context.h"
#include "nanox_error.h"

#define CTX cmd.transferSecContext

static void init_amount(void)
{
  uint8_t len;

  len = pretty_print_hnt(global.fullStr, CTX.amount);
  global.fullStr_len = len;
}

static void init_recipient(void)
{
  size_t output_len;
  cx_sha256_t hash;
  unsigned char hash_buffer[32];
  // use the G_io_apdu buffer as a scratchpad to minimize stack usage
  uint8_t * address_with_check = G_io_apdu_buffer;

  for(uint8_t i=0; i<2; i++){
    // display recipient address on screen
    memmove(address_with_check, CTX.payee, 34);

    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
    memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
    btchip_encode_base58(address_with_check, 38, global.fullStr, &output_len);
    global.fullStr[output_len] = '\0';
    global.fullStr_len = output_len;
  }
}

static void init_fee(void)
{
  uint8_t len;

  // display data credit transaction fee
  len = bin2dec(global.fullStr, CTX.fee);
  global.fullStr_len = len;
  global.fullStr[len] = '\0';
}

static void validate_transaction(bool isApproved)
{
  int adpu_tx;

  if (isApproved) {
    adpu_tx = create_helium_transfer_sec(global.account_index);
    io_exchange_with_code(SW_OK, adpu_tx);
  }
  else {
    // make sure there's no data in the office
    memset(G_io_apdu_buffer, 0, IO_APDU_BUFFER_SIZE);
    // send a single 0 byte to differentiate from app not running
    io_exchange_with_code(SW_OK, 1);
  }

  ui_idle();
}

UX_STEP_NOCB_INIT(
    ux_txfer_sec_display_amount,
    bnnn_paging,
    init_amount(),
    {
#ifdef HELIUM_TESTNET
      .title = "Amount TST",
#else
      .title = "Amount HST",
#endif
	.text = (char *)global.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_sec_display_recipient_address,
    bnnn_paging,
    init_recipient(),
    {
      .title = "Recipient Address",
      .text = (char *)global.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_sec_display_fee,
    bnnn_paging,
    init_fee(),
    {
      .title = "Data Credit Fee",
      .text = (char *)global.fullStr
    });

UX_STEP_CB(
    ux_txfer_sec_sign_approve,
    nn,
    validate_transaction(true),
    {
      "Sign transaction?",
      "YES"
    });

UX_STEP_CB(
    ux_txfer_sec_sign_decline,
    nn,
    validate_transaction(false),
    {
      "Sign transaction?",
      "NO"
    });


UX_DEF(ux_txfer_sec_sign_transaction_flow,
       &ux_txfer_sec_display_amount,
       &ux_txfer_sec_display_recipient_address,
       &ux_txfer_sec_display_fee,
       &ux_txfer_sec_sign_approve,
       &ux_txfer_sec_sign_decline
);

static void ui_sign_transaction(void)
{
  if(G_ux.stack_count == 0) {
    ux_stack_push();
  }
  ux_flow_init(0, ux_txfer_sec_sign_transaction_flow, NULL);
}


void handle_sign_transfer_sec_txn(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags,
                                  __attribute__((unused)) volatile unsigned int *tx) {
    if (save_transfer_sec_context(p1, p2, dataBuffer, dataLength, &CTX)) {
        ui_sign_transaction();
    } else {
        ui_displayError();
    }
	*flags |= IO_ASYNCH_REPLY;
}

#endif