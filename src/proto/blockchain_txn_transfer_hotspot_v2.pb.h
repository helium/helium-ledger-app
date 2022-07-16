/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_HELIUM_BLOCKCHAIN_TXN_TRANSFER_HOTSPOT_V2_PB_H_INCLUDED
#define PB_HELIUM_BLOCKCHAIN_TXN_TRANSFER_HOTSPOT_V2_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _helium_blockchain_txn_transfer_hotspot_v2 { 
    pb_callback_t gateway; 
    pb_callback_t owner; 
    pb_callback_t owner_signature; 
    pb_callback_t new_owner; 
    uint64_t fee; 
    /* gateway nonce */
    uint64_t nonce; 
} helium_blockchain_txn_transfer_hotspot_v2;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define helium_blockchain_txn_transfer_hotspot_v2_init_default {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, 0, 0}
#define helium_blockchain_txn_transfer_hotspot_v2_init_zero {{{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define helium_blockchain_txn_transfer_hotspot_v2_gateway_tag 1
#define helium_blockchain_txn_transfer_hotspot_v2_owner_tag 2
#define helium_blockchain_txn_transfer_hotspot_v2_owner_signature_tag 3
#define helium_blockchain_txn_transfer_hotspot_v2_new_owner_tag 4
#define helium_blockchain_txn_transfer_hotspot_v2_fee_tag 5
#define helium_blockchain_txn_transfer_hotspot_v2_nonce_tag 6

/* Struct field encoding specification for nanopb */
#define helium_blockchain_txn_transfer_hotspot_v2_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    gateway,           1) \
X(a, CALLBACK, SINGULAR, BYTES,    owner,             2) \
X(a, CALLBACK, SINGULAR, BYTES,    owner_signature,   3) \
X(a, CALLBACK, SINGULAR, BYTES,    new_owner,         4) \
X(a, STATIC,   SINGULAR, UINT64,   fee,               5) \
X(a, STATIC,   SINGULAR, UINT64,   nonce,             6)
#define helium_blockchain_txn_transfer_hotspot_v2_CALLBACK pb_default_field_callback
#define helium_blockchain_txn_transfer_hotspot_v2_DEFAULT NULL

extern const pb_msgdesc_t helium_blockchain_txn_transfer_hotspot_v2_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define helium_blockchain_txn_transfer_hotspot_v2_fields &helium_blockchain_txn_transfer_hotspot_v2_msg

/* Maximum encoded size of messages (where known) */
/* helium_blockchain_txn_transfer_hotspot_v2_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
