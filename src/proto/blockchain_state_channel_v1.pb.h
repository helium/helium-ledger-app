/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_HELIUM_BLOCKCHAIN_STATE_CHANNEL_V1_PB_H_INCLUDED
#define PB_HELIUM_BLOCKCHAIN_STATE_CHANNEL_V1_PB_H_INCLUDED
#include <pb.h>
#include "packet.pb.h"
#include "region.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _helium_blockchain_state_channel_state_v1 { 
    helium_blockchain_state_channel_state_v1_open = 0, 
    helium_blockchain_state_channel_state_v1_closed = 1 
} helium_blockchain_state_channel_state_v1;

/* Struct definitions */
typedef struct _helium_blockchain_state_channel_diff_append_summary_v1 { 
    pb_callback_t client_pubkeybin; 
    uint64_t num_packets; 
    uint64_t num_dcs; 
} helium_blockchain_state_channel_diff_append_summary_v1;

typedef struct _helium_blockchain_state_channel_diff_update_summary_v1 { 
    uint64_t client_index; 
    uint64_t add_packets; 
    uint64_t add_dcs; 
} helium_blockchain_state_channel_diff_update_summary_v1;

typedef struct _helium_blockchain_state_channel_diff_v1 { 
    pb_callback_t id; 
    uint64_t add_nonce; 
    pb_callback_t signature; 
    pb_callback_t diffs; 
} helium_blockchain_state_channel_diff_v1;

typedef struct _helium_blockchain_state_channel_offer_v1 { 
    bool has_routing;
    helium_routing_information routing; 
    pb_callback_t packet_hash; 
    uint64_t payload_size; 
    uint32_t fcnt; 
    pb_callback_t hotspot; 
    pb_callback_t signature; 
    helium_region region; 
    bool req_diff; 
} helium_blockchain_state_channel_offer_v1;

typedef struct _helium_blockchain_state_channel_packet_v1 { 
    bool has_packet;
    helium_packet packet; 
    pb_callback_t hotspot; 
    pb_callback_t signature; 
    helium_region region; 
    uint64_t hold_time; 
} helium_blockchain_state_channel_packet_v1;

typedef struct _helium_blockchain_state_channel_rejection_v1 { 
    uint32_t reject; 
    pb_callback_t packet_hash; 
} helium_blockchain_state_channel_rejection_v1;

typedef struct _helium_blockchain_state_channel_response_v1 { 
    bool accepted; 
    bool has_downlink;
    helium_packet downlink; 
} helium_blockchain_state_channel_response_v1;

typedef struct _helium_blockchain_state_channel_summary_v1 { 
    pb_callback_t client_pubkeybin; 
    uint64_t num_packets; 
    uint64_t num_dcs; 
} helium_blockchain_state_channel_summary_v1;

typedef struct _helium_blockchain_state_channel_v1 { 
    pb_callback_t id; 
    pb_callback_t owner; 
    uint64_t credits; 
    uint64_t nonce; 
    pb_callback_t summaries; 
    pb_callback_t root_hash; 
    /* This is unused but we can't remove it yet */
    pb_callback_t skewed; 
    helium_blockchain_state_channel_state_v1 state; 
    uint64_t expire_at_block; 
    pb_callback_t signature; 
} helium_blockchain_state_channel_v1;

/* DEPRECATED */
typedef struct _helium_blockchain_state_channel_banner_v1 { 
    bool has_sc;
    helium_blockchain_state_channel_v1 sc; 
} helium_blockchain_state_channel_banner_v1;

typedef struct _helium_blockchain_state_channel_diff_entry_v1 { 
    pb_size_t which_entry;
    union {
        helium_blockchain_state_channel_diff_append_summary_v1 append;
        helium_blockchain_state_channel_diff_update_summary_v1 add;
    } entry; 
} helium_blockchain_state_channel_diff_entry_v1;

typedef struct _helium_blockchain_state_channel_purchase_v1 { 
    bool has_sc;
    helium_blockchain_state_channel_v1 sc; 
    pb_callback_t hotspot; 
    pb_callback_t packet_hash; 
    helium_region region; 
    bool has_sc_diff;
    helium_blockchain_state_channel_diff_v1 sc_diff; 
} helium_blockchain_state_channel_purchase_v1;

typedef struct _helium_blockchain_state_channel_message_v1 { 
    pb_size_t which_msg;
    union {
        helium_blockchain_state_channel_response_v1 response;
        helium_blockchain_state_channel_packet_v1 packet;
        helium_blockchain_state_channel_offer_v1 offer;
        helium_blockchain_state_channel_purchase_v1 purchase;
        helium_blockchain_state_channel_banner_v1 banner;
        helium_blockchain_state_channel_rejection_v1 reject;
    } msg; 
} helium_blockchain_state_channel_message_v1;


/* Helper constants for enums */
#define _helium_blockchain_state_channel_state_v1_MIN helium_blockchain_state_channel_state_v1_open
#define _helium_blockchain_state_channel_state_v1_MAX helium_blockchain_state_channel_state_v1_closed
#define _helium_blockchain_state_channel_state_v1_ARRAYSIZE ((helium_blockchain_state_channel_state_v1)(helium_blockchain_state_channel_state_v1_closed+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define helium_blockchain_state_channel_summary_v1_init_default {{{NULL}, NULL}, 0, 0}
#define helium_blockchain_state_channel_v1_init_default {{{NULL}, NULL}, {{NULL}, NULL}, 0, 0, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, _helium_blockchain_state_channel_state_v1_MIN, 0, {{NULL}, NULL}}
#define helium_blockchain_state_channel_response_v1_init_default {0, false, helium_packet_init_default}
#define helium_blockchain_state_channel_packet_v1_init_default {false, helium_packet_init_default, {{NULL}, NULL}, {{NULL}, NULL}, _helium_region_MIN, 0}
#define helium_blockchain_state_channel_offer_v1_init_default {false, helium_routing_information_init_default, {{NULL}, NULL}, 0, 0, {{NULL}, NULL}, {{NULL}, NULL}, _helium_region_MIN, 0}
#define helium_blockchain_state_channel_purchase_v1_init_default {false, helium_blockchain_state_channel_v1_init_default, {{NULL}, NULL}, {{NULL}, NULL}, _helium_region_MIN, false, helium_blockchain_state_channel_diff_v1_init_default}
#define helium_blockchain_state_channel_diff_v1_init_default {{{NULL}, NULL}, 0, {{NULL}, NULL}, {{NULL}, NULL}}
#define helium_blockchain_state_channel_diff_entry_v1_init_default {0, {helium_blockchain_state_channel_diff_append_summary_v1_init_default}}
#define helium_blockchain_state_channel_diff_append_summary_v1_init_default {{{NULL}, NULL}, 0, 0}
#define helium_blockchain_state_channel_diff_update_summary_v1_init_default {0, 0, 0}
#define helium_blockchain_state_channel_banner_v1_init_default {false, helium_blockchain_state_channel_v1_init_default}
#define helium_blockchain_state_channel_rejection_v1_init_default {0, {{NULL}, NULL}}
#define helium_blockchain_state_channel_message_v1_init_default {0, {helium_blockchain_state_channel_response_v1_init_default}}
#define helium_blockchain_state_channel_summary_v1_init_zero {{{NULL}, NULL}, 0, 0}
#define helium_blockchain_state_channel_v1_init_zero {{{NULL}, NULL}, {{NULL}, NULL}, 0, 0, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, _helium_blockchain_state_channel_state_v1_MIN, 0, {{NULL}, NULL}}
#define helium_blockchain_state_channel_response_v1_init_zero {0, false, helium_packet_init_zero}
#define helium_blockchain_state_channel_packet_v1_init_zero {false, helium_packet_init_zero, {{NULL}, NULL}, {{NULL}, NULL}, _helium_region_MIN, 0}
#define helium_blockchain_state_channel_offer_v1_init_zero {false, helium_routing_information_init_zero, {{NULL}, NULL}, 0, 0, {{NULL}, NULL}, {{NULL}, NULL}, _helium_region_MIN, 0}
#define helium_blockchain_state_channel_purchase_v1_init_zero {false, helium_blockchain_state_channel_v1_init_zero, {{NULL}, NULL}, {{NULL}, NULL}, _helium_region_MIN, false, helium_blockchain_state_channel_diff_v1_init_zero}
#define helium_blockchain_state_channel_diff_v1_init_zero {{{NULL}, NULL}, 0, {{NULL}, NULL}, {{NULL}, NULL}}
#define helium_blockchain_state_channel_diff_entry_v1_init_zero {0, {helium_blockchain_state_channel_diff_append_summary_v1_init_zero}}
#define helium_blockchain_state_channel_diff_append_summary_v1_init_zero {{{NULL}, NULL}, 0, 0}
#define helium_blockchain_state_channel_diff_update_summary_v1_init_zero {0, 0, 0}
#define helium_blockchain_state_channel_banner_v1_init_zero {false, helium_blockchain_state_channel_v1_init_zero}
#define helium_blockchain_state_channel_rejection_v1_init_zero {0, {{NULL}, NULL}}
#define helium_blockchain_state_channel_message_v1_init_zero {0, {helium_blockchain_state_channel_response_v1_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define helium_blockchain_state_channel_diff_append_summary_v1_client_pubkeybin_tag 1
#define helium_blockchain_state_channel_diff_append_summary_v1_num_packets_tag 2
#define helium_blockchain_state_channel_diff_append_summary_v1_num_dcs_tag 3
#define helium_blockchain_state_channel_diff_update_summary_v1_client_index_tag 1
#define helium_blockchain_state_channel_diff_update_summary_v1_add_packets_tag 2
#define helium_blockchain_state_channel_diff_update_summary_v1_add_dcs_tag 3
#define helium_blockchain_state_channel_diff_v1_id_tag 1
#define helium_blockchain_state_channel_diff_v1_add_nonce_tag 2
#define helium_blockchain_state_channel_diff_v1_signature_tag 3
#define helium_blockchain_state_channel_diff_v1_diffs_tag 4
#define helium_blockchain_state_channel_offer_v1_routing_tag 1
#define helium_blockchain_state_channel_offer_v1_packet_hash_tag 2
#define helium_blockchain_state_channel_offer_v1_payload_size_tag 3
#define helium_blockchain_state_channel_offer_v1_fcnt_tag 4
#define helium_blockchain_state_channel_offer_v1_hotspot_tag 5
#define helium_blockchain_state_channel_offer_v1_signature_tag 6
#define helium_blockchain_state_channel_offer_v1_region_tag 7
#define helium_blockchain_state_channel_offer_v1_req_diff_tag 8
#define helium_blockchain_state_channel_packet_v1_packet_tag 1
#define helium_blockchain_state_channel_packet_v1_hotspot_tag 2
#define helium_blockchain_state_channel_packet_v1_signature_tag 3
#define helium_blockchain_state_channel_packet_v1_region_tag 4
#define helium_blockchain_state_channel_packet_v1_hold_time_tag 5
#define helium_blockchain_state_channel_rejection_v1_reject_tag 1
#define helium_blockchain_state_channel_rejection_v1_packet_hash_tag 2
#define helium_blockchain_state_channel_response_v1_accepted_tag 1
#define helium_blockchain_state_channel_response_v1_downlink_tag 4
#define helium_blockchain_state_channel_summary_v1_client_pubkeybin_tag 1
#define helium_blockchain_state_channel_summary_v1_num_packets_tag 2
#define helium_blockchain_state_channel_summary_v1_num_dcs_tag 3
#define helium_blockchain_state_channel_v1_id_tag 1
#define helium_blockchain_state_channel_v1_owner_tag 2
#define helium_blockchain_state_channel_v1_credits_tag 3
#define helium_blockchain_state_channel_v1_nonce_tag 4
#define helium_blockchain_state_channel_v1_summaries_tag 5
#define helium_blockchain_state_channel_v1_root_hash_tag 6
#define helium_blockchain_state_channel_v1_skewed_tag 7
#define helium_blockchain_state_channel_v1_state_tag 8
#define helium_blockchain_state_channel_v1_expire_at_block_tag 9
#define helium_blockchain_state_channel_v1_signature_tag 10
#define helium_blockchain_state_channel_banner_v1_sc_tag 1
#define helium_blockchain_state_channel_diff_entry_v1_append_tag 1
#define helium_blockchain_state_channel_diff_entry_v1_add_tag 2
#define helium_blockchain_state_channel_purchase_v1_sc_tag 1
#define helium_blockchain_state_channel_purchase_v1_hotspot_tag 2
#define helium_blockchain_state_channel_purchase_v1_packet_hash_tag 3
#define helium_blockchain_state_channel_purchase_v1_region_tag 4
#define helium_blockchain_state_channel_purchase_v1_sc_diff_tag 5
#define helium_blockchain_state_channel_message_v1_response_tag 2
#define helium_blockchain_state_channel_message_v1_packet_tag 4
#define helium_blockchain_state_channel_message_v1_offer_tag 5
#define helium_blockchain_state_channel_message_v1_purchase_tag 6
#define helium_blockchain_state_channel_message_v1_banner_tag 7
#define helium_blockchain_state_channel_message_v1_reject_tag 8

/* Struct field encoding specification for nanopb */
#define helium_blockchain_state_channel_summary_v1_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    client_pubkeybin,   1) \
X(a, STATIC,   SINGULAR, UINT64,   num_packets,       2) \
X(a, STATIC,   SINGULAR, UINT64,   num_dcs,           3)
#define helium_blockchain_state_channel_summary_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_summary_v1_DEFAULT NULL

#define helium_blockchain_state_channel_v1_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    id,                1) \
X(a, CALLBACK, SINGULAR, BYTES,    owner,             2) \
X(a, STATIC,   SINGULAR, UINT64,   credits,           3) \
X(a, STATIC,   SINGULAR, UINT64,   nonce,             4) \
X(a, CALLBACK, REPEATED, MESSAGE,  summaries,         5) \
X(a, CALLBACK, SINGULAR, BYTES,    root_hash,         6) \
X(a, CALLBACK, SINGULAR, BYTES,    skewed,            7) \
X(a, STATIC,   SINGULAR, UENUM,    state,             8) \
X(a, STATIC,   SINGULAR, UINT64,   expire_at_block,   9) \
X(a, CALLBACK, SINGULAR, BYTES,    signature,        10)
#define helium_blockchain_state_channel_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_v1_DEFAULT NULL
#define helium_blockchain_state_channel_v1_summaries_MSGTYPE helium_blockchain_state_channel_summary_v1

#define helium_blockchain_state_channel_response_v1_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     accepted,          1) \
X(a, STATIC,   OPTIONAL, MESSAGE,  downlink,          4)
#define helium_blockchain_state_channel_response_v1_CALLBACK NULL
#define helium_blockchain_state_channel_response_v1_DEFAULT NULL
#define helium_blockchain_state_channel_response_v1_downlink_MSGTYPE helium_packet

#define helium_blockchain_state_channel_packet_v1_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  packet,            1) \
X(a, CALLBACK, SINGULAR, BYTES,    hotspot,           2) \
X(a, CALLBACK, SINGULAR, BYTES,    signature,         3) \
X(a, STATIC,   SINGULAR, UENUM,    region,            4) \
X(a, STATIC,   SINGULAR, UINT64,   hold_time,         5)
#define helium_blockchain_state_channel_packet_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_packet_v1_DEFAULT NULL
#define helium_blockchain_state_channel_packet_v1_packet_MSGTYPE helium_packet

#define helium_blockchain_state_channel_offer_v1_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  routing,           1) \
X(a, CALLBACK, SINGULAR, BYTES,    packet_hash,       2) \
X(a, STATIC,   SINGULAR, UINT64,   payload_size,      3) \
X(a, STATIC,   SINGULAR, UINT32,   fcnt,              4) \
X(a, CALLBACK, SINGULAR, BYTES,    hotspot,           5) \
X(a, CALLBACK, SINGULAR, BYTES,    signature,         6) \
X(a, STATIC,   SINGULAR, UENUM,    region,            7) \
X(a, STATIC,   SINGULAR, BOOL,     req_diff,          8)
#define helium_blockchain_state_channel_offer_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_offer_v1_DEFAULT NULL
#define helium_blockchain_state_channel_offer_v1_routing_MSGTYPE helium_routing_information

#define helium_blockchain_state_channel_purchase_v1_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  sc,                1) \
X(a, CALLBACK, SINGULAR, BYTES,    hotspot,           2) \
X(a, CALLBACK, SINGULAR, BYTES,    packet_hash,       3) \
X(a, STATIC,   SINGULAR, UENUM,    region,            4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  sc_diff,           5)
#define helium_blockchain_state_channel_purchase_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_purchase_v1_DEFAULT NULL
#define helium_blockchain_state_channel_purchase_v1_sc_MSGTYPE helium_blockchain_state_channel_v1
#define helium_blockchain_state_channel_purchase_v1_sc_diff_MSGTYPE helium_blockchain_state_channel_diff_v1

#define helium_blockchain_state_channel_diff_v1_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    id,                1) \
X(a, STATIC,   SINGULAR, UINT64,   add_nonce,         2) \
X(a, CALLBACK, SINGULAR, BYTES,    signature,         3) \
X(a, CALLBACK, REPEATED, MESSAGE,  diffs,             4)
#define helium_blockchain_state_channel_diff_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_diff_v1_DEFAULT NULL
#define helium_blockchain_state_channel_diff_v1_diffs_MSGTYPE helium_blockchain_state_channel_diff_entry_v1

#define helium_blockchain_state_channel_diff_entry_v1_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (entry,append,entry.append),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (entry,add,entry.add),   2)
#define helium_blockchain_state_channel_diff_entry_v1_CALLBACK NULL
#define helium_blockchain_state_channel_diff_entry_v1_DEFAULT NULL
#define helium_blockchain_state_channel_diff_entry_v1_entry_append_MSGTYPE helium_blockchain_state_channel_diff_append_summary_v1
#define helium_blockchain_state_channel_diff_entry_v1_entry_add_MSGTYPE helium_blockchain_state_channel_diff_update_summary_v1

#define helium_blockchain_state_channel_diff_append_summary_v1_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    client_pubkeybin,   1) \
X(a, STATIC,   SINGULAR, UINT64,   num_packets,       2) \
X(a, STATIC,   SINGULAR, UINT64,   num_dcs,           3)
#define helium_blockchain_state_channel_diff_append_summary_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_diff_append_summary_v1_DEFAULT NULL

#define helium_blockchain_state_channel_diff_update_summary_v1_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT64,   client_index,      1) \
X(a, STATIC,   SINGULAR, UINT64,   add_packets,       2) \
X(a, STATIC,   SINGULAR, UINT64,   add_dcs,           3)
#define helium_blockchain_state_channel_diff_update_summary_v1_CALLBACK NULL
#define helium_blockchain_state_channel_diff_update_summary_v1_DEFAULT NULL

#define helium_blockchain_state_channel_banner_v1_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  sc,                1)
#define helium_blockchain_state_channel_banner_v1_CALLBACK NULL
#define helium_blockchain_state_channel_banner_v1_DEFAULT NULL
#define helium_blockchain_state_channel_banner_v1_sc_MSGTYPE helium_blockchain_state_channel_v1

#define helium_blockchain_state_channel_rejection_v1_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   reject,            1) \
X(a, CALLBACK, SINGULAR, BYTES,    packet_hash,       2)
#define helium_blockchain_state_channel_rejection_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_state_channel_rejection_v1_DEFAULT NULL

#define helium_blockchain_state_channel_message_v1_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (msg,response,msg.response),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (msg,packet,msg.packet),   4) \
X(a, STATIC,   ONEOF,    MESSAGE,  (msg,offer,msg.offer),   5) \
X(a, STATIC,   ONEOF,    MESSAGE,  (msg,purchase,msg.purchase),   6) \
X(a, STATIC,   ONEOF,    MESSAGE,  (msg,banner,msg.banner),   7) \
X(a, STATIC,   ONEOF,    MESSAGE,  (msg,reject,msg.reject),   8)
#define helium_blockchain_state_channel_message_v1_CALLBACK NULL
#define helium_blockchain_state_channel_message_v1_DEFAULT NULL
#define helium_blockchain_state_channel_message_v1_msg_response_MSGTYPE helium_blockchain_state_channel_response_v1
#define helium_blockchain_state_channel_message_v1_msg_packet_MSGTYPE helium_blockchain_state_channel_packet_v1
#define helium_blockchain_state_channel_message_v1_msg_offer_MSGTYPE helium_blockchain_state_channel_offer_v1
#define helium_blockchain_state_channel_message_v1_msg_purchase_MSGTYPE helium_blockchain_state_channel_purchase_v1
#define helium_blockchain_state_channel_message_v1_msg_banner_MSGTYPE helium_blockchain_state_channel_banner_v1
#define helium_blockchain_state_channel_message_v1_msg_reject_MSGTYPE helium_blockchain_state_channel_rejection_v1

extern const pb_msgdesc_t helium_blockchain_state_channel_summary_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_response_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_packet_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_offer_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_purchase_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_diff_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_diff_entry_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_diff_append_summary_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_diff_update_summary_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_banner_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_rejection_v1_msg;
extern const pb_msgdesc_t helium_blockchain_state_channel_message_v1_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define helium_blockchain_state_channel_summary_v1_fields &helium_blockchain_state_channel_summary_v1_msg
#define helium_blockchain_state_channel_v1_fields &helium_blockchain_state_channel_v1_msg
#define helium_blockchain_state_channel_response_v1_fields &helium_blockchain_state_channel_response_v1_msg
#define helium_blockchain_state_channel_packet_v1_fields &helium_blockchain_state_channel_packet_v1_msg
#define helium_blockchain_state_channel_offer_v1_fields &helium_blockchain_state_channel_offer_v1_msg
#define helium_blockchain_state_channel_purchase_v1_fields &helium_blockchain_state_channel_purchase_v1_msg
#define helium_blockchain_state_channel_diff_v1_fields &helium_blockchain_state_channel_diff_v1_msg
#define helium_blockchain_state_channel_diff_entry_v1_fields &helium_blockchain_state_channel_diff_entry_v1_msg
#define helium_blockchain_state_channel_diff_append_summary_v1_fields &helium_blockchain_state_channel_diff_append_summary_v1_msg
#define helium_blockchain_state_channel_diff_update_summary_v1_fields &helium_blockchain_state_channel_diff_update_summary_v1_msg
#define helium_blockchain_state_channel_banner_v1_fields &helium_blockchain_state_channel_banner_v1_msg
#define helium_blockchain_state_channel_rejection_v1_fields &helium_blockchain_state_channel_rejection_v1_msg
#define helium_blockchain_state_channel_message_v1_fields &helium_blockchain_state_channel_message_v1_msg

/* Maximum encoded size of messages (where known) */
/* helium_blockchain_state_channel_summary_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_packet_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_offer_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_purchase_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_diff_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_diff_entry_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_diff_append_summary_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_banner_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_rejection_v1_size depends on runtime parameters */
/* helium_blockchain_state_channel_message_v1_size depends on runtime parameters */
#if defined(helium_packet_size)
#define helium_blockchain_state_channel_response_v1_size (8 + helium_packet_size)
#endif
#define helium_blockchain_state_channel_diff_update_summary_v1_size 33

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
