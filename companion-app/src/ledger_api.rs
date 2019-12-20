use helium_api::Client;
use ledger::*;

use crate::{hnt::Hnt, pubkeybin::PubKeyBin, pubkeybin::B58, Result, payment_txn::PaymentTxn};
use byteorder::{LittleEndian as LE, WriteBytesExt};
use helium_proto::txn::{TxnPaymentV1, Wrapper};
use prost::Message;
use std::error;
use std::fmt;

const INS_GET_PUBLIC_KEY: u8 = 0x02;
const INS_SIGN_PAYMENT_TXN: u8 = 0x08;

// This parameter indicates whether the ledgers screen display the public key or not
// Thus, the `pay` function can do the Adpu transaction quietly to get the public key
pub enum PubkeyDisplay {
    Off = 0,
    On = 1,
}

#[derive(Debug)]
enum Error {
    CouldNotFindLedger,
    AppNotRunning,
}

pub fn get_pubkey(display: PubkeyDisplay) -> Result<PubKeyBin> {
    let ledger = LedgerApp::new()?;
    exchange_get_pubkey(&ledger, display)
}

fn exchange_get_pubkey(ledger: &LedgerApp, display: PubkeyDisplay) -> Result<PubKeyBin> {
    let get_public_key = ApduCommand {
        cla: 0xe0,
        ins: INS_GET_PUBLIC_KEY,
        p1: display as u8,
        p2: 0x00,
        length: 0,
        data: Vec::new(),
    };

    let public_key_result = read_from_ledger(ledger, get_public_key)?;
    // TODO: verify validity before returning by checking the sha256 checksum
    Ok(PubKeyBin::copy_from_slice(&public_key_result.data[1..34]))
}

pub enum PayResponse {
    Txn(PaymentTxn),
    InsufficientBalance(u64, u64), // provides balance and send request
    UserDeniedTransaction,
}

pub fn pay(payee: String, amount: Hnt) -> Result<PayResponse> {
    let ledger = LedgerApp::new()?;
    let client = Client::new();
    let fee: u64 = 0;
    let mut data: Vec<u8> = Vec::new();

    // get nonce
    let keypair = exchange_get_pubkey(&ledger, PubkeyDisplay::Off)?;
    let account = client.get_account(&keypair.to_b58()?)?;
    let nonce: u64 = account.nonce + 1;

    if account.balance < amount.to_bones() {
        return Ok(PayResponse::InsufficientBalance(
            account.balance,
            amount.to_bones(),
        ));
    }

    // serlialize payee
    let payee_bin = PubKeyBin::from_b58(payee)?;
    data.write_u64::<LE>(amount.to_bones())?;
    data.write_u64::<LE>(fee)?;
    data.write_u64::<LE>(nonce)?;

    data.push(0);
    data.extend(payee_bin.0.iter());

    let exchange_pay_txn = ApduCommand {
        cla: 0xe0,
        ins: INS_SIGN_PAYMENT_TXN,
        p1: 0x00,
        p2: 0x00,
        length: 0,
        data,
    };

    let exchange_pay_tx_result = read_from_ledger(&ledger, exchange_pay_txn)?;

    if exchange_pay_tx_result.data.len() == 1 {
        return Ok(PayResponse::UserDeniedTransaction);
    }

    let txn = TxnPaymentV1::decode(exchange_pay_tx_result.data.clone())?;

    client.submit_txn(Wrapper::Payment(txn.clone()))?;
    Ok(PayResponse::Txn(txn.into()))
}

impl error::Error for Error {
    fn source(&self) -> Option<&(dyn error::Error + 'static)> {
        None
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Error::CouldNotFindLedger => {
                write!(f, "Could not find ledger. Is it disconnected or locked?")
            }
            Error::AppNotRunning => write!(
                f,
                "Ledger is running but Helium application does not appear to be running"
            ),
        }
    }
}

fn read_from_ledger(
    ledger: &LedgerApp,
    command: ApduCommand,
) -> std::result::Result<ledger::ApduAnswer, Error> {
    let answer = ledger
        .exchange(command)
        .or(Err(Error::CouldNotFindLedger))?;

    if answer.data.is_empty() {
        Err(Error::AppNotRunning)
    } else {
        Ok(answer)
    }
}
