#include "helium.h"
#include "pb.h"
#include "pb_encode.h"
#include "../proto/blockchain_txn.pb.h"
#include "save_context.h"

// we only allow one payment
// key + 2 x uint64 + some buffer
#define MAX_PAYMENT_SIZE (SIZEOF_HELIUM_KEY+2*4+23)

uint32_t create_helium_pay_txn(uint8_t account){
    paymentContext_t * ctx = &cmd.paymentContext;
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

    unsigned char payment[MAX_PAYMENT_SIZE];
    memset(payment, 0, MAX_PAYMENT_SIZE);
    uint8_t len_payments;

    // first encode the submessage
    ostream = pb_ostream_from_buffer(payment, sizeof(payment));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_payment_payee_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)&ctx->payee[1], SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_VARINT, helium_payment_amount_tag);
    pb_encode_varint(&ostream, ctx->amount);

    if(ctx->memo) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_payment_memo_tag);
        pb_encode_varint(&ostream, ctx->memo);
    }
    len_payments = ostream.bytes_written;

    // now do the top-level message
    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_payment_v2_payer_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)payer, SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_payment_v2_payments_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)payment, len_payments);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_payment_v2_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    if(ctx->nonce) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_payment_v2_nonce_tag);
        pb_encode_varint(&ostream, ctx->nonce);
    }

    if (!sign_tx(signature, account, G_io_apdu_buffer, ostream.bytes_written)){
        return 0;
    }

    // redo the top level message only
    ostream = pb_ostream_from_buffer(G_io_apdu_buffer, sizeof(G_io_apdu_buffer));
    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_payment_v2_payer_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)payer, SIZEOF_HELIUM_KEY);

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_payment_v2_payments_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)payment, len_payments);

    if(ctx->fee) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_payment_v2_fee_tag);
        pb_encode_varint(&ostream, ctx->fee);
    }

    if(ctx->nonce) {
        pb_encode_tag(&ostream, PB_WT_VARINT, helium_blockchain_txn_payment_v2_nonce_tag);
        pb_encode_varint(&ostream, ctx->nonce);
    }

    pb_encode_tag(&ostream, PB_WT_STRING, helium_blockchain_txn_payment_v2_signature_tag);
    pb_encode_string(&ostream, (const pb_byte_t*)signature, SIZEOF_SIGNATURE);

    return ostream.bytes_written;
}
