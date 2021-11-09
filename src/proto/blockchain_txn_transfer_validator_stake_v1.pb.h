/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_HELIUM_BLOCKCHAIN_TXN_TRANSFER_VALIDATOR_STAKE_V1_PB_H_INCLUDED
#define PB_HELIUM_BLOCKCHAIN_TXN_TRANSFER_VALIDATOR_STAKE_V1_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _helium_blockchain_txn_transfer_validator_stake_v1 { 
    pb_callback_t old_address; 
    pb_callback_t new_address; 
    pb_callback_t old_owner; 
    pb_callback_t new_owner; 
    pb_callback_t old_owner_signature; 
    pb_callback_t new_owner_signature; 
    uint64_t fee; 
    uint64_t stake_amount; /* for accounting */
    /* optional amount (in bones) the new owner is transferring to the old owner */
    uint64_t payment_amount; 
} helium_blockchain_txn_transfer_validator_stake_v1;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define helium_blockchain_txn_transfer_validator_stake_v1_init_default {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, 0, 0, 0}
#define helium_blockchain_txn_transfer_validator_stake_v1_init_zero {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define helium_blockchain_txn_transfer_validator_stake_v1_old_address_tag 1
#define helium_blockchain_txn_transfer_validator_stake_v1_new_address_tag 2
#define helium_blockchain_txn_transfer_validator_stake_v1_old_owner_tag 3
#define helium_blockchain_txn_transfer_validator_stake_v1_new_owner_tag 4
#define helium_blockchain_txn_transfer_validator_stake_v1_old_owner_signature_tag 5
#define helium_blockchain_txn_transfer_validator_stake_v1_new_owner_signature_tag 6
#define helium_blockchain_txn_transfer_validator_stake_v1_fee_tag 7
#define helium_blockchain_txn_transfer_validator_stake_v1_stake_amount_tag 8
#define helium_blockchain_txn_transfer_validator_stake_v1_payment_amount_tag 9

/* Struct field encoding specification for nanopb */
#define helium_blockchain_txn_transfer_validator_stake_v1_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    old_address,       1) \
X(a, CALLBACK, SINGULAR, BYTES,    new_address,       2) \
X(a, CALLBACK, SINGULAR, BYTES,    old_owner,         3) \
X(a, CALLBACK, SINGULAR, BYTES,    new_owner,         4) \
X(a, CALLBACK, SINGULAR, BYTES,    old_owner_signature,   5) \
X(a, CALLBACK, SINGULAR, BYTES,    new_owner_signature,   6) \
X(a, STATIC,   SINGULAR, UINT64,   fee,               7) \
X(a, STATIC,   SINGULAR, UINT64,   stake_amount,      8) \
X(a, STATIC,   SINGULAR, UINT64,   payment_amount,    9)
#define helium_blockchain_txn_transfer_validator_stake_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_txn_transfer_validator_stake_v1_DEFAULT NULL

extern const pb_msgdesc_t helium_blockchain_txn_transfer_validator_stake_v1_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define helium_blockchain_txn_transfer_validator_stake_v1_fields &helium_blockchain_txn_transfer_validator_stake_v1_msg

/* Maximum encoded size of messages (where known) */
/* helium_blockchain_txn_transfer_validator_stake_v1_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif