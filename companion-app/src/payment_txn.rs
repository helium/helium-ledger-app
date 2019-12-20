use crate::hnt::Hnt;
use crate::pubkeybin::PubKeyBin;
use helium_proto::txn::TxnPaymentV1;
use prost::Message;
use sha2::{Digest, Sha256};

pub struct PaymentTxn {
    pub payer: PubKeyBin,
    pub payee: PubKeyBin,
    pub amount: Hnt,
    pub fee: u64,
    pub nonce: u64,
    pub signature: [u8; 64],
    pub hash: Hash,
}

pub struct Hash([u8; 33]);

impl ToString for Hash {
    fn to_string(&self) -> String {
        bs58::encode(self.0.as_ref()).with_check().into_string()
    }
}

impl From<TxnPaymentV1> for PaymentTxn {
    fn from(txn: TxnPaymentV1) -> Self {
        let payer = PubKeyBin::copy_from_slice(&txn.payer.as_slice());
        let payee = PubKeyBin::copy_from_slice(&txn.payee.as_slice());
        let mut signature: [u8; 64] = [0; 64];
        signature.copy_from_slice(&txn.signature.as_slice()[..64]);

        let amount = Hnt::from_bones(txn.amount);

        let mut txn_copy = txn.clone();
        // clear the signature so we can compute the hash
        txn_copy.signature = Vec::new();
        let mut hasher = Sha256::new();
        // write input message
        let mut buf = Vec::new();
        txn_copy.encode(&mut buf).unwrap();
        hasher.input(buf.as_slice());
        let result = hasher.result();

        let mut hash = [0u8; 33];
        hash[1..].copy_from_slice(&result);

        PaymentTxn {
            payer,
            payee,
            signature,
            amount,
            hash: Hash(hash),
            nonce: txn.nonce,
            fee: txn.fee,
        }
    }
}