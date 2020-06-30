use super::Result;
use helium_api::{blockchain_txn::Txn, BlockchainTxn, BlockchainTxnPaymentV1};
use prost::Message;

const TXN_FEE_SIGNATURE_SIZE: usize = 64;
const TXN_FEE_MULTIPLIER: u64 = 5_000;

fn calculate_txn_fee(payload_size: usize) -> u64 {
    let dc_payload_size = 24;
    // integer div/ceil from: https://stackoverflow.com/a/2745086
    ((payload_size + dc_payload_size - 1) / dc_payload_size) as u64
}

pub trait Fee {
    fn in_envelope(&self) -> BlockchainTxn;
    fn fee(&self) -> Result<u64>;
}

impl Fee for BlockchainTxnPaymentV1 {
    fn in_envelope(&self) -> BlockchainTxn {
        BlockchainTxn {
            txn: Some(Txn::Payment(self.clone())),
        }
    }

    fn fee(&self) -> Result<u64> {
        let mut txn = self.clone();
        txn.fee = 0;
        txn.signature = vec![0; TXN_FEE_SIGNATURE_SIZE];
        let mut buf = Vec::new();
        txn.in_envelope().encode(&mut buf)?;
        Ok(calculate_txn_fee(buf.len()) * TXN_FEE_MULTIPLIER)
    }
}

#[test]
fn payment_v1_fee() {
    let payer: [u8; 33] = [0xFF; 33];
    let payee: [u8; 33] = [0xFF; 33];
    let mut txn = BlockchainTxnPaymentV1 {
        payee: payee.to_vec(),
        payer: payer.to_vec(),
        amount: 0,
        nonce: 1,
        fee: 0,
        signature: vec![],
    };
    txn.amount = 10_000;
    assert_eq!(txn.fee().unwrap(), 30_000);

    txn.amount = 16_383;
    assert_eq!(txn.fee().unwrap(), 30_000);

    txn.amount = 16_384;
    assert_eq!(txn.fee().unwrap(), 35_000);
}

pub struct Hash([u8; 32]);

impl ToString for Hash {
    fn to_string(&self) -> String {
        base64::encode_config(self.0.as_ref(), base64::URL_SAFE)
    }
}
