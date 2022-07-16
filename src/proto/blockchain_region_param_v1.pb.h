/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_HELIUM_BLOCKCHAIN_REGION_PARAM_V1_PB_H_INCLUDED
#define PB_HELIUM_BLOCKCHAIN_REGION_PARAM_V1_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _helium_RegionSpreading { 
    helium_RegionSpreading_SF_INVALID = 0, 
    helium_RegionSpreading_SF7 = 1, 
    helium_RegionSpreading_SF8 = 2, 
    helium_RegionSpreading_SF9 = 3, 
    helium_RegionSpreading_SF10 = 4, 
    helium_RegionSpreading_SF11 = 5, 
    helium_RegionSpreading_SF12 = 6 
} helium_RegionSpreading;

/* Struct definitions */
typedef struct _helium_blockchain_region_params_v1 { 
    pb_callback_t region_params; 
} helium_blockchain_region_params_v1;

typedef struct _helium_blockchain_region_spreading_v1 { 
    pb_callback_t tagged_spreading; 
} helium_blockchain_region_spreading_v1;

typedef struct _helium_blockchain_region_param_v1 { 
    /* in hertz */
    uint64_t channel_frequency; 
    /* in hertz */
    uint32_t bandwidth; 
    /* in dBi x 10 */
    uint32_t max_eirp; 
    /* list of atoms */
    bool has_spreading;
    helium_blockchain_region_spreading_v1 spreading; 
} helium_blockchain_region_param_v1;

typedef struct _helium_tagged_spreading { 
    helium_RegionSpreading region_spreading; 
    uint32_t max_packet_size; 
} helium_tagged_spreading;


/* Helper constants for enums */
#define _helium_RegionSpreading_MIN helium_RegionSpreading_SF_INVALID
#define _helium_RegionSpreading_MAX helium_RegionSpreading_SF12
#define _helium_RegionSpreading_ARRAYSIZE ((helium_RegionSpreading)(helium_RegionSpreading_SF12+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define helium_blockchain_region_params_v1_init_default {{{NULL}, NULL}}
#define helium_tagged_spreading_init_default     {_helium_RegionSpreading_MIN, 0}
#define helium_blockchain_region_spreading_v1_init_default {{{NULL}, NULL}}
#define helium_blockchain_region_param_v1_init_default {0, 0, 0, false, helium_blockchain_region_spreading_v1_init_default}
#define helium_blockchain_region_params_v1_init_zero {{{NULL}, NULL}}
#define helium_tagged_spreading_init_zero        {_helium_RegionSpreading_MIN, 0}
#define helium_blockchain_region_spreading_v1_init_zero {{{NULL}, NULL}}
#define helium_blockchain_region_param_v1_init_zero {0, 0, 0, false, helium_blockchain_region_spreading_v1_init_zero}

/* Field tags (for use in manual encoding/decoding) */
#define helium_blockchain_region_params_v1_region_params_tag 1
#define helium_blockchain_region_spreading_v1_tagged_spreading_tag 1
#define helium_blockchain_region_param_v1_channel_frequency_tag 1
#define helium_blockchain_region_param_v1_bandwidth_tag 2
#define helium_blockchain_region_param_v1_max_eirp_tag 3
#define helium_blockchain_region_param_v1_spreading_tag 4
#define helium_tagged_spreading_region_spreading_tag 1
#define helium_tagged_spreading_max_packet_size_tag 2

/* Struct field encoding specification for nanopb */
#define helium_blockchain_region_params_v1_FIELDLIST(X, a) \
X(a, CALLBACK, REPEATED, MESSAGE,  region_params,     1)
#define helium_blockchain_region_params_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_region_params_v1_DEFAULT NULL
#define helium_blockchain_region_params_v1_region_params_MSGTYPE helium_blockchain_region_param_v1

#define helium_tagged_spreading_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    region_spreading,   1) \
X(a, STATIC,   SINGULAR, UINT32,   max_packet_size,   2)
#define helium_tagged_spreading_CALLBACK NULL
#define helium_tagged_spreading_DEFAULT NULL

#define helium_blockchain_region_spreading_v1_FIELDLIST(X, a) \
X(a, CALLBACK, REPEATED, MESSAGE,  tagged_spreading,   1)
#define helium_blockchain_region_spreading_v1_CALLBACK pb_default_field_callback
#define helium_blockchain_region_spreading_v1_DEFAULT NULL
#define helium_blockchain_region_spreading_v1_tagged_spreading_MSGTYPE helium_tagged_spreading

#define helium_blockchain_region_param_v1_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT64,   channel_frequency,   1) \
X(a, STATIC,   SINGULAR, UINT32,   bandwidth,         2) \
X(a, STATIC,   SINGULAR, UINT32,   max_eirp,          3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  spreading,         4)
#define helium_blockchain_region_param_v1_CALLBACK NULL
#define helium_blockchain_region_param_v1_DEFAULT NULL
#define helium_blockchain_region_param_v1_spreading_MSGTYPE helium_blockchain_region_spreading_v1

extern const pb_msgdesc_t helium_blockchain_region_params_v1_msg;
extern const pb_msgdesc_t helium_tagged_spreading_msg;
extern const pb_msgdesc_t helium_blockchain_region_spreading_v1_msg;
extern const pb_msgdesc_t helium_blockchain_region_param_v1_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define helium_blockchain_region_params_v1_fields &helium_blockchain_region_params_v1_msg
#define helium_tagged_spreading_fields &helium_tagged_spreading_msg
#define helium_blockchain_region_spreading_v1_fields &helium_blockchain_region_spreading_v1_msg
#define helium_blockchain_region_param_v1_fields &helium_blockchain_region_param_v1_msg

/* Maximum encoded size of messages (where known) */
/* helium_blockchain_region_params_v1_size depends on runtime parameters */
/* helium_blockchain_region_spreading_v1_size depends on runtime parameters */
/* helium_blockchain_region_param_v1_size depends on runtime parameters */
#define helium_tagged_spreading_size             8

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
