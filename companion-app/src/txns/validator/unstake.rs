use crate::txns::*;

#[derive(Debug, StructOpt)]
/// Unstake a given validator. The stake will be in a cooldown period after
/// unstaking before the HNT is returned to the owning wallet.
pub struct Cmd {
    /// Address of the validator to unstake
    address: PublicKey,

    /// The amount of HNT of the original stake
    #[structopt(long)]
    stake_amount: Option<Hnt>,

    /// The stake release block height. This should be at least the current
    /// block height plus the cooldown period, and 5-10 blocks to allow for
    /// chain processing delays.
    #[structopt(long)]
    stake_release_height: u64,

    /// Manually set the fee to pay for the transaction
    #[structopt(long)]
    fee: Option<u64>,
}

impl Cmd {
    pub(crate) async fn run(
        self,
        opts: Opts,
        _version: Version,
    ) -> Result<Option<(String, Network)>> {
        match ledger(opts, self).await? {
            Response::Txn(_txn, hash, network) => Ok(Some((hash, network))),
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
    unstake: Cmd,
) -> Result<Response<BlockchainTxnUnstakeValidatorV1>> {
    let ledger = get_ledger_transport(&opts).await?;

    // get account from API so we can get nonce and balance
    let owner = get_pubkey(opts.account, &ledger, PubkeyDisplay::Off).await?;

    let client = Client::new_with_base_url(api_url(owner.network));

    let mut txn = BlockchainTxnUnstakeValidatorV1 {
        owner: owner.to_vec(),
        address: unstake.address.to_vec(),
        stake_amount: if let Some(stake_amount) = unstake.stake_amount {
            u64::from(stake_amount)
        } else {
            helium_api::validators::get(&client, &unstake.address.to_string())
                .await?
                .stake
        },
        stake_release_height: unstake.stake_release_height,
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

    print_proposed_txn(&txn)?;

    let cmd = txn.apdu_serialize(opts.account)?;
    let exchange_pay_tx_result = read_from_ledger(&ledger, cmd).await?;

    if exchange_pay_tx_result.data.len() == 1 {
        return Ok(Response::UserDeniedTransaction);
    }

    let txn = BlockchainTxnUnstakeValidatorV1::decode(exchange_pay_tx_result.data.as_slice())?;
    let envelope = txn.in_envelope();
    // submit the signed tansaction to the API
    let pending_txn_status = submit_txn(&client, &envelope).await?;

    Ok(Response::Txn(txn, pending_txn_status.hash, owner.network))
}

pub fn print_proposed_txn(txn: &BlockchainTxnUnstakeValidatorV1) -> Result {
    let owner = PublicKey::try_from(txn.owner.clone())?;
    let units = match owner.network {
        Network::TestNet => "TNT",
        Network::MainNet => "HNT",
    };

    let mut table = Table::new();
    println!("Creating the following stake transaction:");
    table.add_row(row![
        &format!("Unstake Amount {}", units),
        "Stake Release Height",
        "Validator Address",
        "DC Fee"
    ]);
    table.add_row(row![
        Hnt::from(txn.stake_amount),
        txn.stake_release_height,
        PublicKey::try_from(txn.address.clone())?,
        txn.fee
    ]);
    table.printstd();

    println!(
        "WARNING: do not use this output as the source of truth. Instead, rely \
    on the Ledger Display"
    );

    println!(
        "\nINFO: After unstaking, a validator can not access the staked amount\n\
                until the enetered stake release height (approx. five months)."
    );
    Ok(())
}
