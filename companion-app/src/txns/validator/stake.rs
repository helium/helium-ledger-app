use crate::txns::*;

#[derive(Debug, StructOpt)]
pub struct Cmd {
    /// Amount to stake
    pub amount: Hnt,
    /// Address of the validator to stake
    pub address: PublicKey,
}

impl Cmd {
    pub(crate) async fn run(
        self,
        opts: Opts,
        _version: Version,
    ) -> Result<Option<(String, Network)>> {

        match ledger(opts, self).await? {
            Response::Txn(_txn, hash, network) => {
                Ok(Some((hash, network)))
            }
            Response::InsufficientBalance(balance, send_request) => {
                println!(
                    "Account balance insufficient. {} HNT on account but attempting to stake {}",
                    balance, send_request,
                );
                Err(Error::txn())
            }
            Response::UserDeniedTransaction => {
                println!("Transaction not confirmed");
                Err(Error::txn())
            }
        }
    }
}


pub(crate) async fn ledger(
    opts: Opts,
    stake: Cmd,
) -> Result<Response<BlockchainTxnStakeValidatorV1>> {
    let ledger = TransportNativeHID::new()?;

    // get account from API so we can get nonce and balance
    let owner = exchange_get_pubkey(opts.account, &ledger, PubkeyDisplay::Off)?;

    let client = Client::new_with_base_url(api_url(owner.network));

    let account = accounts::get(&client, &owner.to_string()).await?;

    if account.balance.get_decimal() < stake.amount.get_decimal() {
        return Ok(Response::InsufficientBalance(account.balance, stake.amount));
    }

    let mut txn = BlockchainTxnStakeValidatorV1 {
        owner: owner.to_vec(),
        address: stake.address.to_vec(),
        stake: u64::from(stake.amount),
        fee: 0,
        owner_signature: vec![],
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
    let exchange_pay_tx_result = read_from_ledger(&ledger, cmd)?;

    if exchange_pay_tx_result.data.len() == 1 {
        return Ok(Response::UserDeniedTransaction);
    }

    let txn = BlockchainTxnStakeValidatorV1::decode(exchange_pay_tx_result.data.as_slice())?;
    let envelope = txn.in_envelope();
    // submit the signed tansaction to the API
    let pending_txn_status = submit_txn(&client, &envelope).await?;

    Ok(Response::Txn(txn, pending_txn_status.hash, owner.network))
}

fn print_proposed_transaction(stake: &BlockchainTxnStakeValidatorV1) -> Result {
    let address =  PublicKey::try_from(stake.address.clone())?;
    let units = match address.network {
        Network::TestNet => "TNT",
        Network::MainNet => "HNT",
    };

    let mut table = Table::new();
    println!("Creating the following stake transaction:");
    table.add_row(row![
        &format!("Stake Amount {}", units),
        "Validator Address",
        "DC Fee"
    ]);
    table.add_row(row![Hnt::from(stake.stake), address, stake.fee]);
    table.printstd();
    println!(
        "WARNING: do not use this output as the source of truth. Instead, rely \
    on the Ledger Display"
    );
    Ok(())
}