/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_HELIUM_BLOCKCHAIN_TXN_TRANSFER_HOTSPOT_V1_PB_H_INCLUDED
#define PB_HELIUM_BLOCKCHAIN_TXN_TRANSFER_HOTSPOT_V1_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _helium_blockchain_txn_transfer_hotspot_v1 { 
    pb_callback_t gateway; 
    pb_callback_t seller; 
    pb_callback_t buyer; 
    pb_callback_t seller_signature; 
    pb_callback_t buyer_signature; 
    uint64_t buyer_nonce; /* buyer's next payment nonce, required even if payment is 0 */
    uint64_t amount_to_seller; /* in bones not raw HNT */
    uint64_t fee; 
} helium_blockchain_txn_transfer_hotspot_v1;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define helium_blockchain_txn_transfer_hotspot_v1_init_default {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, 0, 0, 0}
#define helium_blockchain_txn_transfer_hotspot_v1_init_zero {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define helium_blockchain_txn_transfer_hotspot_v1_gateway_tag 1
#define helium_blockchain_txn_transfer_hotspot_v1_seller_tag 2
#define helium_blockchain_txn_transfer_hotspot_v1_buyer_tag 3
#define helium_blockchain_txn_transfer_hotspot_v1_seller_signature_tag 4
#define helium_blockchain_txn_transfer_hotspot_v1_buyer_signature_tag 5
#define helium_blockchain_txn_transfer_hotspot_v1_buyer_nonce_tag 6
#define helium_blockchain_txn_transfer_hotspot_v1_amount_to_seller_tag 7
#define helium_blockchain_txn_transfer_hotspot_v1_fee_tag 8

/* Struct field encoding specification for nanopb */
#define helium_blockchain_txn_transfer_hotspot_v1_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    gateway,           1) \
X(a, CALLBACK, SINGULAR, BYTES,    seller,            2) \
X(a, CALLBACK, SINGULAR, BYTES,    buyer,             3) \
X(a, CALLBACK, SINGULAR, BYTES,    seller_signature,   4) \
X(a, CALLBACK, SINGULAR, BYTES,    buyer_signature,   5) \
X(a, STATIC,   SINGULAR, UINT64,   buyer_nonce,       6) \
X(a, STATIC,   SINGULAR, UINT64,   amount_to_seller,   7) \
X(a, STATIC,   SINGULAR, UINT64,   fee,               8)
#define helium_blockchain_txn_transfer_hotspot_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_txn_transfer_hotspot_v1_DEFAULT NULL

extern const pb_msgdesc_t helium_blockchain_txn_transfer_hotspot_v1_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define helium_blockchain_txn_transfer_hotspot_v1_fields &helium_blockchain_txn_transfer_hotspot_v1_msg

/* Maximum encoded size of messages (where known) */
/* helium_blockchain_txn_transfer_hotspot_v1_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif