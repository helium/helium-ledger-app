#include "helium.h"
#include "pb.h"
#include "pb_encode.h"
#include "../proto/blockchain_txn.pb.h"

uint32_t create_helium_transfer_validator_txn(uint8_t account){
    transferValidatorContext_t * ctx = &global.transferValidatorContext;
    pb_ostream_t ostream;

    unsigned char owner[SIZEOF_HELIUM_KEY];
#ifdef HELIUM_TESTNET
    owner[0] = NETTYPE_TEST | KEYTYPE_ED25519;;
#else
    owner[0] = NETTYPE_MAIN | KEYTYPE_ED25519;;
#endif
    get_pubkey_bytes(account, &owner[1]);

    bool is_new_owner = (memcmp(owner, &ctx->new_owner[1], SIZEOF_HELIUM_KEY) == 0);
    bool is_old_owner = (memcmp(owner, &ctx->old_owner[1], SIZEOF_HELIUM_KEY) == 0);

    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_old_address_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->old_address[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_new_address_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->new_address[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_old_owner_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->old_owner[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_new_owner_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->new_owner[1], SIZEOF_HELIUM_KEY);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_transfer_validator_stake_v1_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    if(ctx->stake_amount) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_transfer_validator_stake_v1_stake_amount_tag);
        pb_encode_varint(&ostream, ctx->stake_amount);
    }

    if(ctx->payment_amount) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_transfer_validator_stake_v1_payment_amount_tag);
        pb_encode_varint(&ostream, ctx->payment_amount);
    }

    unsigned char signature[SIZEOF_SIGNATURE];
    memset(signature, 0, SIZEOF_SIGNATURE);
    sign_tx(signature, account, G_io_apdu_buffer, ostream.bytes_written);

    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_old_address_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->old_address[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_new_address_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->new_address[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_old_owner_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->old_owner[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_new_owner_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->new_owner[1], SIZEOF_HELIUM_KEY);

    // to avoid two APDU transactions, we only write the signature once
    // the companion app must make the copy
    // ie: companion app must check if is_old_owner && is_new_owner; if so, copy signature
    if(is_old_owner){
        pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_old_owner_signature_tag);
        pb_encode_string(&ostream, (const pb_byte_t*)signature, SIZEOF_SIGNATURE);
    }

    if(is_new_owner && !is_old_owner){
        pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_transfer_validator_stake_v1_new_owner_signature_tag);
        pb_encode_string(&ostream, (const pb_byte_t*)signature, SIZEOF_SIGNATURE);
    }

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_transfer_validator_stake_v1_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    if(ctx->stake_amount) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_transfer_validator_stake_v1_stake_amount_tag);
        pb_encode_varint(&ostream, ctx->stake_amount);
    }

    if(ctx->payment_amount) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_transfer_validator_stake_v1_payment_amount_tag);
        pb_encode_varint(&ostream, ctx->payment_amount);
    }

    return ostream.bytes_written;
}
