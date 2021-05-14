#include "helium.h"
#include "pb.h"
#include "pb_encode.h"
#include "../proto/blockchain_txn.pb.h"

uint32_t create_helium_unstake_txn(uint8_t account){
    unstakeValidatorContext_t * ctx = &global.unstakeValidatorContext;
    pb_ostream_t ostream;

    unsigned char owner[SIZEOF_HELIUM_KEY];
#ifdef HELIUM_TESTNET
    owner[0] = NETTYPE_TEST | KEYTYPE_ED25519;;
#else
    owner[0] = NETTYPE_MAIN | KEYTYPE_ED25519;;
#endif
    get_pubkey_bytes(account, &owner[1]);

    unsigned char signature[SIZEOF_SIGNATURE];
    memset(signature, 0, SIZEOF_SIGNATURE);

    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_unstake_validator_v1_address_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->address[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_unstake_validator_v1_owner_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&owner, SIZEOF_HELIUM_KEY);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_stake_amount_tag);
    pb_encode_varint(&ostream, ctx->stake_amount);

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_stake_release_height_tag);
    pb_encode_varint(&ostream, ctx->stake_release_height);

    sign_tx(signature, account, G_io_apdu_buffer, ostream.bytes_written);

    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_unstake_validator_v1_address_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->address[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_unstake_validator_v1_owner_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&owner, SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_unstake_validator_v1_owner_signature_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)signature, SIZEOF_SIGNATURE);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_stake_amount_tag);
    pb_encode_varint(&ostream, ctx->stake_amount);    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_unstake_validator_v1_address_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->address[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_unstake_validator_v1_owner_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&owner, SIZEOF_HELIUM_KEY);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_stake_amount_tag);
    pb_encode_varint(&ostream, ctx->stake_amount);

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_unstake_validator_v1_stake_release_height_tag);
    pb_encode_varint(&ostream, ctx->stake_release_height);

    return ostream.bytes_written;
}
