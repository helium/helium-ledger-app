use crate::*;
use byteorder::{LittleEndian as LE, WriteBytesExt};
use helium_api::{accounts, Client, Hnt};
use helium_proto::{
    BlockchainTxnPaymentV1, BlockchainTxnStakeValidatorV1, BlockchainTxnTransferValidatorStakeV1,
    BlockchainTxnUnstakeValidatorV1,
};
use helium_wallet::{
    cmd::get_txn_fees,
    traits::{TxnEnvelope, TxnFee},
    keypair::{PublicKey, Network}
};
use ledger::*;
use ledger_apdu::{APDUAnswer, APDUCommand};
use prost::Message;
use std::convert::TryFrom;

pub mod balance;
pub mod pay;
pub mod serializer;
pub mod validator;
pub use serializer::*;

const RETURN_CODE_OK: u16 = 0x9000;

// This parameter indicates whether the ledgers screen display the public key or not
// Thus, the `pay` function can do the Adpu transaction quietly to get the public key
#[derive(Copy, Clone)]
pub enum PubkeyDisplay {
    Off = 0,
    On = 1,
}

pub(crate) fn get_app_version() -> Result<Version> {
    let ledger = TransportNativeHID::new()?;
    let request = VersionRequest.apdu_serialize(0)?;
    let read = read_from_ledger(&ledger, request)?;
    let data = read.data;
    if data.len() == 3 && read.retcode == RETURN_CODE_OK {
        Ok(Version::from_bytes([data[0], data[1], data[2]]))
    } else {
        Err(Error::VersionError)
    }
}

pub(crate) fn get_pubkey(account: u8, display: PubkeyDisplay) -> Result<PublicKey> {
    let ledger = TransportNativeHID::new()?;
    exchange_get_pubkey(account, &ledger, display)
}

fn exchange_get_pubkey(
    account: u8,
    ledger: &TransportNativeHID,
    display: PubkeyDisplay,
) -> Result<PublicKey> {
    let cmd = PubkeyRequest { display }.apdu_serialize(account)?;
    let public_key_result = read_from_ledger(ledger, cmd)?;
    Ok(PublicKey::try_from(&public_key_result.data[1..34])?)
}

pub enum Response<T> {
    Txn(T, String, Network),
    InsufficientBalance(Hnt, Hnt), // provides balance and send request
    UserDeniedTransaction,
}

fn read_from_ledger(ledger: &TransportNativeHID, command: APDUCommand) -> Result<APDUAnswer> {
    let answer = ledger
        .exchange(&command)
        .or(Err(Error::CouldNotFindLedger))?;

    if answer.data.is_empty() {
        Err(Error::AppNotRunning)
    } else {
        Ok(answer)
    }
}
