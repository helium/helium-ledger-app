#include "bolos_target.h"

#ifdef HAVE_UX_FLOW

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include "helium.h"
#include "helium_ux.h"
#include "save_context.h"

#define CTX global.transferValidatorContext

static void init_stake_amount(void)
{
  uint8_t len;

  len = pretty_print_hnt(CTX.fullStr, CTX.stake_amount);
  CTX.fullStr_len = len;
}

static void init_payment_amount(void)
{
  uint8_t len;

  len = pretty_print_hnt(CTX.fullStr, CTX.payment_amount);
  CTX.fullStr_len = len;
}

static void init_old_owner(void)
{
  size_t output_len;
  cx_sha256_t hash;
  unsigned char hash_buffer[32];
  // use the G_io_apdu buffer as a scratchpad to minimize stack usage
  uint8_t * address_with_check = G_io_apdu_buffer;

  for(uint8_t i=0; i<2; i++){
    os_memmove(address_with_check, CTX.old_owner, 34);

    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
    os_memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
    btchip_encode_base58(address_with_check, 38, CTX.fullStr, &output_len);
    CTX.fullStr[output_len] = '\0';
    CTX.fullStr_len = output_len;
  }
}

static void init_new_owner(void)
{
  size_t output_len;
  cx_sha256_t hash;
  unsigned char hash_buffer[32];
  // use the G_io_apdu buffer as a scratchpad to minimize stack usage
  uint8_t * address_with_check = G_io_apdu_buffer;

  for(uint8_t i=0; i<2; i++){
    os_memmove(address_with_check, CTX.new_owner, 34);

    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
    os_memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
    btchip_encode_base58(address_with_check, 38, CTX.fullStr, &output_len);
    CTX.fullStr[output_len] = '\0';
    CTX.fullStr_len = output_len;
  }
}

static void init_old_address(void)
{
  size_t output_len;
  cx_sha256_t hash;
  unsigned char hash_buffer[32];
  // use the G_io_apdu buffer as a scratchpad to minimize stack usage
  uint8_t * address_with_check = G_io_apdu_buffer;

  for(uint8_t i=0; i<2; i++){
    os_memmove(address_with_check, CTX.old_address, 34);

    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
    os_memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
    btchip_encode_base58(address_with_check, 38, CTX.fullStr, &output_len);
    CTX.fullStr[output_len] = '\0';
    CTX.fullStr_len = output_len;
  }
}

static void init_new_address(void)
{
  size_t output_len;
  cx_sha256_t hash;
  unsigned char hash_buffer[32];
  // use the G_io_apdu buffer as a scratchpad to minimize stack usage
  uint8_t * address_with_check = G_io_apdu_buffer;

  for(uint8_t i=0; i<2; i++){
    os_memmove(address_with_check, CTX.old_address, 34);

    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
    os_memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
    btchip_encode_base58(address_with_check, 38, CTX.fullStr, &output_len);
    CTX.fullStr[output_len] = '\0';
    CTX.fullStr_len = output_len;
  }
}

static void init_fee(void)
{
  uint8_t len;
  len = bin2dec(CTX.fullStr, CTX.fee);
  CTX.fullStr_len = len;
  CTX.fullStr[len] = '\0';
}

static void validate_transaction(bool isApproved)
{
  int adpu_tx;

  if (isApproved) {
    adpu_tx = create_helium_transfer_validator_txn(CTX.account_index);
    io_exchange_with_code(SW_OK, adpu_tx);
  }
  else {
    // make sure there's no data in the office
    memset(G_io_apdu_buffer, 0, IO_APDU_BUFFER_SIZE);
    // send a single 0 byte to differentiate from app not running
    io_exchange_with_code(SW_OK, 1);
  }

  // Go back to main menu
  ui_idle();
}

UX_STEP_NOCB_INIT(
    ux_txfer_display_stake_amount,
    bnnn_paging,
    init_stake_amount(),
    {
#ifdef HELIUM_TESTNET
      .title = "Transfer TNT Stake",
#else
      .title = "Transfer HNT Stake",
#endif
	.text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_display_payment_amount,
    bnnn_paging,
    init_payment_amount(),
    {
#ifdef HELIUM_TESTNET
      .title = "TNT Paid to Old Owner",
#else
      .title = "HNT Paid to Old Owner",
#endif
	.text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_display_old_owner,
    bnnn_paging,
    init_old_owner(),
    {
      .title = "Old Owner",
      .text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_display_new_owner,
    bnnn_paging,
    init_new_owner(),
    {
      .title = "New Owner",
      .text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_display_old_address,
    bnnn_paging,
    init_old_address(),
    {
      .title = "Old Address",
      .text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_display_new_address,
    bnnn_paging,
    init_new_address(),
    {
      .title = "New Addres",
      .text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_txfer_display_fee,
    bnnn_paging,
    init_fee(),
    {
      .title = "Data Credit Fee",
      .text = (char *)CTX.fullStr
    });

UX_STEP_CB(
    ux_txfer_sign_approve,
    nn,
    validate_transaction(true),
    {
      "Sign transaction?",
      "YES"
    });

UX_STEP_CB(
    ux_txfer_sign_decline,
    nn,
    validate_transaction(false),
    {
      "Sign transaction?",
      "NO"
    });


UX_DEF(ux_txfer_sign_transaction_flow,
       &ux_txfer_display_stake_amount,
       &ux_txfer_display_payment_amount,
       &ux_txfer_display_old_owner,
       &ux_txfer_display_new_owner,
       &ux_txfer_display_old_address,
       &ux_txfer_display_new_address,
       &ux_txfer_sign_approve,
       &ux_txfer_sign_decline
);

static void ui_sign_transaction(void)
{
  if(G_ux.stack_count == 0) {
    ux_stack_push();
  }
  ux_flow_init(0, ux_txfer_sign_transaction_flow, NULL);
}


void handle_validator_txfer(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
    save_validator_transfer_context(p1, p2, dataBuffer, dataLength, &CTX);
	ui_sign_transaction();
	*flags |= IO_ASYNCH_REPLY;
}

#endif
