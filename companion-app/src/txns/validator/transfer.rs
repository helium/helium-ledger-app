use crate::txns::*;
use helium_wallet::traits::B64;

#[derive(Debug, StructOpt)]
/// Create or accept a validator transfer with this Ledger wallet
pub enum Cmd {
    Create(Create),
    Accept(Accept),
}

#[derive(Debug, StructOpt)]
/// Create a validator transfer transaction with a Ledger wallet as as the current (old) owner or
/// new owner. If either owner is not specified, the select Ledger wallet account is assumed to be
/// that/those owner(s).
pub struct Create {
    /// The validator to transfer the stake from
    #[structopt(long)]
    old_address: PublicKey,

    /// The validator to transfer the stake to
    #[structopt(long)]
    new_address: PublicKey,

    /// The new owner of the transferred validator and stake. If not present
    /// the new owner is assumed to be the same as the current owner as defined
    /// on the blockchain.
    #[structopt(long)]
    new_owner: Option<PublicKey>,

    /// The current (old) owner of the transferred validator and stake. If not present
    /// the old owner is set to the public key of the given wallet.
    #[structopt(long)]
    old_owner: Option<PublicKey>,

    /// The amount of HNT to transfer from the new to the old owner as part of
    /// the stake transfer
    #[structopt(long, default_value = "0")]
    payment: Hnt,

    /// The amount of HNT of the original stake
    #[structopt(long)]
    stake_amount: Option<Hnt>,
}

#[derive(Debug, StructOpt)]
/// Accept a given stake transfer transaction by signing it and committing to
/// the API. The transaction is signed as either (or both) the new owner or the
/// old owner if the owner keys match the public key of the given wallet.
pub struct Accept {
    /// Base64 encoded transaction to sign. If no transaction if given
    /// stdin is read for the transaction. Note that the stdin feature
    /// only works if the wallet password is set in the
    /// HELIUM_WALLET_PASSWORD environment variable
    #[structopt(name = "TRANSACTION")]
    txn: Option<Transaction>,
}

impl Cmd {
    pub(crate) async fn run(
        self,
        opts: Opts,
        _version: Version,
    ) -> Result<Option<(String, Network)>> {
        match self {
            Cmd::Create(create) => match ledger_create(opts, create).await? {
                Some(Response::Txn(_txn, hash, network)) => Ok(Some((hash, network))),
                Some(Response::InsufficientBalance(balance, send_request)) => {
                    println!(
                        "Account balance insufficient. {} HNT on account but attempting to stake {}",
                        balance, send_request,
                    );
                    Err(Error::txn())
                }
                Some(Response::UserDeniedTransaction) => {
                    println!("Transaction not confirmed");
                    Err(Error::txn())
                }
                _ => Ok(None),
            },
            Cmd::Accept(accept) => match ledger_accept(opts, accept).await? {
                Some(Response::Txn(_txn, hash, network)) => Ok(Some((hash, network))),
                Some(Response::InsufficientBalance(balance, send_request)) => {
                    println!(
                        "Account balance insufficient. {} HNT on account but attempting to stake {}",
                        balance, send_request,
                    );
                    Err(Error::txn())
                }
                Some(Response::UserDeniedTransaction) => {
                    println!("Transaction not confirmed");
                    Err(Error::txn())
                }
                _ => Ok(None),
            },
        }
    }
}

pub(crate) async fn ledger_create(
    opts: Opts,
    txfer_stake: Create,
) -> Result<Option<Response<BlockchainTxnTransferValidatorStakeV1>>> {
    let ledger = TransportNativeHID::new()?;
    let this_wallet = exchange_get_pubkey(opts.account, &ledger, PubkeyDisplay::Off)?;

    // old_owner defaults to self if not input
    let old_owner = if let Some(old_owner) = txfer_stake.old_owner {
        old_owner
    } else {
        this_wallet.clone()
    };

    // old_owner defaults to self if not input
    let new_owner = if let Some(new_owner) = txfer_stake.new_owner {
        new_owner
    } else {
        this_wallet.clone()
    };

    // verify that we are one of the parties involved
    if this_wallet != old_owner && this_wallet != new_owner {
        println!("ERROR: Selected Ledger account is neither current nor new owner of validator!");
        return Ok(None);
    }

    let client = Client::new_with_base_url(api_url(old_owner.network));
    // calculate fee
    let mut txn = BlockchainTxnTransferValidatorStakeV1 {
        new_owner: new_owner.to_vec(),
        old_owner: old_owner.to_vec(),
        new_address: txfer_stake.new_address.to_vec(),
        old_address: txfer_stake.old_address.to_vec(),
        fee: 0,
        payment_amount: u64::from(txfer_stake.payment),
        stake_amount: if let Some(stake_amount) = txfer_stake.stake_amount {
            u64::from(stake_amount)
        } else {
            helium_api::validators::get(&client, &txfer_stake.old_address.to_string())
                .await?
                .stake
        },
        new_owner_signature: vec![],
        old_owner_signature: vec![],
    };

    txn.fee = txn
        .txn_fee(
            &get_txn_fees(&client)
                .await
                .map_err(|_| Error::getting_fees())?,
        )
        .map_err(|_| Error::getting_fees())?;

    print_proposed_transaction(&txn)?;

    let cmd = txn.apdu_serialize(opts.account)?;
    let result = read_from_ledger(&ledger, cmd)?;
    if result.data.len() == 1 {
        return Ok(Some(Response::UserDeniedTransaction));
    }
    let data = result.data;

    let mut txn = BlockchainTxnTransferValidatorStakeV1::decode(data.as_slice())?;

    // A create transfer can only be submitted if we are both old and new owners
    if old_owner == new_owner {
        // The APDU txn only passes the signature once as old_owner so as to avoid multiple APDU
        // frames. We make the copy here to accommodate
        txn.new_owner_signature = txn.old_owner_signature.clone();

        // submit the signed transaction to the API
        let pending_txn_status = submit_txn(&client, &txn.in_envelope()).await?;

        Ok(Some(Response::Txn(
            txn,
            pending_txn_status.hash,
            old_owner.network,
        )))
    } else {
        println!("Provide the following base64 output to the counter-party for counter-signing: ");
        println!("{}", txn.in_envelope().to_b64().unwrap());
        Ok(None)
    }
}

pub(crate) async fn ledger_accept(
    opts: Opts,
    accept: Accept,
) -> Result<Option<Response<BlockchainTxnTransferValidatorStakeV1>>> {
    let read = read_txn(&accept.txn)?;
    let mut input_txn = BlockchainTxnTransferValidatorStakeV1::from_envelope(&read)
        .map_err(|_| Error::into_envelope())?;
    print_proposed_transaction(&input_txn)?;

    let old_owner = PublicKey::try_from(input_txn.old_owner.clone())?;
    let new_owner = PublicKey::try_from(input_txn.new_owner.clone())?;

    // get Ledger account so that we can verify relevance
    let ledger = TransportNativeHID::new()?;
    let this_wallet = exchange_get_pubkey(opts.account, &ledger, PubkeyDisplay::Off)?;

    // verify that we are one of the parties involved
    if this_wallet != old_owner && this_wallet != new_owner {
        println!("ERROR: Selected Ledger account is neither current nor new owner of validator!");
        return Ok(None);
    }

    let cmd = input_txn.apdu_serialize(opts.account)?;
    let result = read_from_ledger(&ledger, cmd)?;

    if result.data.len() == 1 {
        return Ok(Some(Response::UserDeniedTransaction));
    }

    // Decode the transaction returned by the ledger
    let txn = BlockchainTxnTransferValidatorStakeV1::decode(result.data.as_slice())?;

    // We move the signatures returned by the Ledger for whatever roles it fulfills
    // We support an unsigned transactions where we are both old and new owners
    if this_wallet == old_owner {
        // The APDU txn only passes the signature once as old_owner so as to avoid multiple APDU
        // frames. We make the copy here to accommodate
        if this_wallet == new_owner {
            input_txn.new_owner_signature = txn.old_owner_signature.clone();
        }
        input_txn.old_owner_signature = txn.old_owner_signature;
    } else if this_wallet == new_owner {
        input_txn.new_owner_signature = txn.new_owner_signature;
    }

    // submit the signed transaction to the API
    let client = Client::new_with_base_url(api_url(old_owner.network));
    let pending_txn_status = submit_txn(&client, &input_txn.in_envelope()).await?;

    Ok(Some(Response::Txn(
        input_txn,
        pending_txn_status.hash,
        old_owner.network,
    )))
}

#[derive(Debug, Clone)]
pub struct Transaction(BlockchainTxn);

impl std::str::FromStr for Transaction {
    type Err = Error;

    fn from_str(s: &str) -> Result<Self> {
        Ok(Self(
            BlockchainTxn::from_b64(s).map_err(|_| Error::from_b64())?,
        ))
    }
}

use std::io;
fn read_txn(txn: &Option<Transaction>) -> Result<BlockchainTxn> {
    match txn {
        Some(txn) => Ok(txn.0.clone()),
        None => {
            let mut buffer = String::new();
            io::stdin().read_line(&mut buffer)?;
            Ok(buffer.trim().parse::<Transaction>()?.0)
        }
    }
}

fn print_proposed_transaction(txn: &BlockchainTxnTransferValidatorStakeV1) -> Result {
    let old_address = PublicKey::try_from(txn.old_address.clone())?;

    let units = match old_address.network {
        Network::TestNet => "TNT",
        Network::MainNet => "HNT",
    };

    let mut table = Table::new();
    println!("Constructing transfer stake transaction:");
    table.add_row(row!["Old Owner", "New Owner",]);
    table.add_row(row![
        PublicKey::try_from(txn.old_owner.clone())?,
        PublicKey::try_from(txn.new_owner.clone())?,
    ]);
    table.printstd();
    table = Table::new();
    table.add_row(row!["Old Address", "New Address",]);
    table.add_row(row![
        old_address,
        PublicKey::try_from(txn.new_address.clone())?,
    ]);
    table.printstd();
    table = Table::new();
    table.add_row(row![
        "Payment to Old Owner",
        &format!("Stake Amount {}", units),
        "DC Fee"
    ]);
    table.add_row(row![Hnt::from(txn.payment_amount), Hnt::from(txn.stake_amount), txn.fee]);
    table.printstd();
    println!(
        "WARNING: do not use this output as the source of truth. Instead, rely \
    on the Ledger Display"
    );
    Ok(())
}
