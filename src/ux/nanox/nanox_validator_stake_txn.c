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

#define CTX global.stakeValidatorContext

static void init_stake_amount(void)
{
  uint8_t len;

  len = pretty_print_hnt(CTX.fullStr, CTX.stake_amount);
  CTX.fullStr_len = len;
}

static void init_stake_address(void)
{
  size_t output_len;
  cx_sha256_t hash;
  unsigned char hash_buffer[32];
  // use the G_io_apdu buffer as a scratchpad to minimize stack usage
  uint8_t * address_with_check = G_io_apdu_buffer;
  
  for(uint8_t i=0; i<2; i++){
    // display recipient address on screen
    memmove(address_with_check, CTX.address, 34);

    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, address_with_check, 34, hash_buffer, 32);
    cx_sha256_init(&hash);
    cx_hash(&hash.header, CX_LAST, hash_buffer, 32, hash_buffer, 32);
    memmove(&address_with_check[34], hash_buffer, SIZE_OF_SHA_CHECKSUM);
    btchip_encode_base58(address_with_check, 38, CTX.fullStr, &output_len);
    CTX.fullStr[output_len] = '\0';
    CTX.fullStr_len = output_len;
    /* UX_DISPLAY(ui_displayRecipient, ui_prepro_displayRecipient); */
  }
}

static void init_fee(void)
{
  uint8_t len;

  // display data credit transaction fee
  len = bin2dec(CTX.fullStr, CTX.fee);
  CTX.fullStr_len = len;
  CTX.fullStr[len] = '\0';
		
  /* UX_DISPLAY(ui_displayFee, ui_prepro_displayFee); */
  /* break; */
}

static void validate_transaction(bool isApproved)
{
  int adpu_tx;

  if (isApproved) {
    adpu_tx = create_helium_stake_txn(CTX.account_index);
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
    ux_display_stake_amount,
    bnnn_paging,
    init_stake_amount(),
    {
#ifdef HELIUM_TESTNET
      .title = "Stake TNT",
#else
      .title = "Stake HNT",
#endif
	.text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_display_stake_address,
    bnnn_paging,
    init_stake_address(),
    {
      .title = "Stake Address",
      .text = (char *)CTX.fullStr
    });

UX_STEP_NOCB_INIT(
    ux_display_stake_fee,
    bnnn_paging,
    init_fee(),
    {
      .title = "Data Credit Fee",
      .text = (char *)CTX.fullStr
    });

UX_STEP_CB(
    ux_sign_stake_approve,
    nn,
    validate_transaction(true),
    {
      "Sign transaction?",
      "YES"
    });

UX_STEP_CB(
    ux_sign_stake_decline,
    nn,
    validate_transaction(false),
    {
      "Sign transaction?",
      "NO"
    });


UX_DEF(ux_sign_transaction_flow,
       &ux_display_stake_amount,
       &ux_display_stake_address,
       &ux_display_stake_fee,
       &ux_sign_stake_approve,
       &ux_sign_stake_decline
);

static void ui_sign_transaction(void)
{
  if(G_ux.stack_count == 0) {
    ux_stack_push();
  }
  ux_flow_init(0, ux_sign_transaction_flow, NULL);
}


void handle_stake_validator_txn(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
    save_stake_validator_context(p1, p2, dataBuffer, dataLength, &CTX);
	ui_sign_transaction();
	*flags |= IO_ASYNCH_REPLY;
}

#endif
