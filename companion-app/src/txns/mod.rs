use crate::*;
use byteorder::{LittleEndian as LE, WriteBytesExt};
use helium_api::{accounts, Client, Hnt};
use helium_proto::{
    BlockchainTxnPaymentV1, BlockchainTxnStakeValidatorV1, BlockchainTxnTransferValidatorStakeV1,
    BlockchainTxnUnstakeValidatorV1,
};
use helium_wallet::{
    cmd::get_txn_fees,
    keypair::{Network, PublicKey},
    traits::{TxnEnvelope, TxnFee},
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

pub(crate) async fn get_ledger_transport(opts: &Opts) -> Result<Box<dyn LedgerTransport>> {
    Ok(if opts.emulator {
        Box::new(ledger_tcp::TransportTcp::new().await?)
    } else {
        Box::new(TransportNativeHID::new()?)
    })
}

pub(crate) async fn get_app_version(opts: &Opts) -> Result<Version> {
    let ledger = get_ledger_transport(opts).await?;
    let request = VersionRequest.apdu_serialize(0)?;
    let read = read_from_ledger(&ledger, request).await?;
    let data = read.data;
    if data.len() == 3 && read.retcode == RETURN_CODE_OK {
        Ok(Version::from_bytes([data[0], data[1], data[2]]))
    } else {
        Err(Error::VersionError)
    }
}
#[allow(clippy::borrowed_box)]
async fn get_pubkey(
    account: u8,
    ledger: &Box<dyn LedgerTransport>,
    display: PubkeyDisplay,
) -> Result<PublicKey> {
    let cmd = PubkeyRequest { display }.apdu_serialize(account)?;
    let public_key_result = read_from_ledger(ledger, cmd).await?;
    Ok(PublicKey::try_from(&public_key_result.data[1..34])?)
}

pub enum Response<T> {
    Txn(T, String, Network),
    InsufficientBalance(Hnt, Hnt), // provides balance and send request
    UserDeniedTransaction,
}

#[allow(clippy::borrowed_box)]
async fn read_from_ledger(
    ledger: &Box<dyn LedgerTransport>,
    command: APDUCommand,
) -> Result<APDUAnswer> {
    let answer = ledger.exchange(&command).await?;

    if answer.data.is_empty() {
        Err(Error::AppNotRunning)
    } else {
        Ok(answer)
    }
}
