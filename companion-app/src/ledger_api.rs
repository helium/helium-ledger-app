use crate::{api_url, submit_txn, Result};
use byteorder::{LittleEndian as LE, WriteBytesExt};
use helium_api::{accounts, Client, Hnt};
use helium_crypto::{public_key::PublicKey, Network};
use helium_proto::BlockchainTxnPaymentV1;
use helium_wallet::{
    cmd::get_txn_fees,
    traits::{TxnEnvelope, TxnFee},
};
use ledger::*;
use ledger_apdu::{APDUAnswer, APDUCommand};
use prost::Message;
use std::{convert::TryFrom, error, fmt, str::FromStr};

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

pub fn get_pubkey(display: PubkeyDisplay) -> Result<PublicKey> {
    let ledger = TransportNativeHID::new()?;
    exchange_get_pubkey(&ledger, display)
}

fn exchange_get_pubkey(ledger: &TransportNativeHID, display: PubkeyDisplay) -> Result<PublicKey> {
    let get_public_key = APDUCommand {
        cla: 0xe0,
        ins: INS_GET_PUBLIC_KEY,
        p1: display as u8,
        p2: 0x00,
        data: Vec::new(),
    };

    let public_key_result = read_from_ledger(ledger, get_public_key)?;
    Ok(PublicKey::try_from(&public_key_result.data[1..34])?)
}

pub enum PayResponse {
    Txn(BlockchainTxnPaymentV1, String),
    InsufficientBalance(Hnt, Hnt), // provides balance and send request
    UserDeniedTransaction,
}

pub(crate) async fn pay(payee: PublicKey, amount: Hnt) -> Result<PayResponse> {
    let ledger = TransportNativeHID::new()?;
    // get nonce
    let pubkey = exchange_get_pubkey(&ledger, PubkeyDisplay::Off)?;

    let client = Client::new_with_base_url(api_url(pubkey.network));
    let mut data: Vec<u8> = Vec::new();

    let account = accounts::get(&client, &pubkey.to_string()).await?;
    let nonce: u64 = account.speculative_nonce + 1;

    if account.balance.get_decimal() < amount.get_decimal() {
        return Ok(PayResponse::InsufficientBalance(account.balance, amount));
    }
    // serialize payer
    let payer = PublicKey::from_str(&account.address)?;

    // calculate fee
    let fee = BlockchainTxnPaymentV1 {
        payee: payee.to_vec(),
        payer: payer.to_vec(),
        amount: u64::from(amount),
        nonce,
        fee: 0,
        signature: vec![],
    }
    .txn_fee(&get_txn_fees(&client).await?)?;

    let units = match payer.network {
        Network::TestNet => "TNT",
        Network::MainNet => "HNT",
    };

    println!("Transaction fee: {} DC (1 DC = $.00001)", fee);
    println!("If account has no DCs, {} will be burned automatically to fund transaction based on current oracle price", units);

    data.write_u64::<LE>(u64::from(amount))?;
    data.write_u64::<LE>(fee)?;
    data.write_u64::<LE>(nonce)?;

    data.push(0);
    data.extend(payee.to_vec());

    let exchange_pay_txn = APDUCommand {
        cla: 0xe0,
        ins: INS_SIGN_PAYMENT_TXN,
        p1: 0x00,
        p2: 0x00,
        data,
    };

    let exchange_pay_tx_result = read_from_ledger(&ledger, exchange_pay_txn)?;

    if exchange_pay_tx_result.data.len() == 1 {
        return Ok(PayResponse::UserDeniedTransaction);
    }

    let txn = BlockchainTxnPaymentV1::decode(exchange_pay_tx_result.data.as_slice())?;
    let envelope = txn.in_envelope();
    // submit the signed tansaction to the API
    let pending_txn_status = submit_txn(&client, &envelope).await?;

    Ok(PayResponse::Txn(txn, pending_txn_status.hash))
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
    ledger: &TransportNativeHID,
    command: APDUCommand,
) -> std::result::Result<APDUAnswer, Error> {
    let answer = ledger
        .exchange(&command)
        .or(Err(Error::CouldNotFindLedger))?;

    if answer.data.is_empty() {
        Err(Error::AppNotRunning)
    } else {
        Ok(answer)
    }
}
