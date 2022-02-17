#include "helium.h"
#include "pb.h"
#include "pb_encode.h"
#include "../proto/blockchain_txn.pb.h"
#include "save_context.h"

uint32_t create_helium_transfer_sec(uint8_t account){
    transferSec_t * ctx = &global.transferSecContext;
    pb_ostream_t ostream;

    unsigned char payer[SIZEOF_HELIUM_KEY];
#ifdef HELIUM_TESTNET
    payer[0] = NETTYPE_TEST | KEYTYPE_ED25519;;
#else
    payer[0] = NETTYPE_MAIN | KEYTYPE_ED25519;;
#endif
    get_pubkey_bytes(account, &payer[1]);

    unsigned char signature[SIZEOF_SIGNATURE];
    memset(signature, 0, SIZEOF_SIGNATURE);

    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_security_exchange_v1_payer_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)payer, SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_security_exchange_v1_payee_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->payee[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_security_exchange_v1_amount_tag);
    pb_encode_varint(&ostream, ctx->amount);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_security_exchange_v1_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    if(ctx->nonce) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_security_exchange_v1_nonce_tag);
        pb_encode_varint(&ostream, ctx->nonce);
    }

    sign_tx(signature, account, G_io_apdu_buffer, ostream.bytes_written);

    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_security_exchange_v1_payer_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)payer, SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_security_exchange_v1_payee_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->payee[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_security_exchange_v1_amount_tag);
    pb_encode_varint(&ostream, ctx->amount);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_security_exchange_v1_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    if(ctx->nonce) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_security_exchange_v1_nonce_tag);
        pb_encode_varint(&ostream, ctx->nonce);
    }

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_security_exchange_v1_signature_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)signature, SIZEOF_SIGNATURE);

    return ostream.bytes_written;
}
