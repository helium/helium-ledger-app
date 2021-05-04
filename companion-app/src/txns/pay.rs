use super::*;

#[derive(Debug, StructOpt)]
pub struct Cmd {
    pub payee: Payee,
}

#[derive(Debug)]
pub struct Payee {
    pub address: PublicKey,
    pub amount: Hnt,
}


impl Cmd {
    pub(crate) async fn run(
        self,
        opts: Opts,
        version: Version,
    ) -> Result<Option<(String, Network)>> {
        if version.major < 2 && opts.account != 0 {
            panic!("Upgrade the Helium Ledger App to use additional wallet accounts");
        };

        match ledger(opts, self).await? {
            Response::Txn(_txn, hash, network) => {
                Ok(Some((hash, network)))
            }
            Response::InsufficientBalance(balance, send_request) => {
                println!(
                    "Account balance insufficient. {} HNT on account but attempting to send {}",
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


async fn ledger(opts: Opts, cmd: Cmd) -> Result<Response<BlockchainTxnPaymentV1>> {
    let ledger = TransportNativeHID::new()?;

    let amount = cmd.payee.amount;
    let payee = cmd.payee.address;

    // get nonce
    let pubkey = exchange_get_pubkey(opts.account, &ledger, PubkeyDisplay::Off)?;
    let client = Client::new_with_base_url(api_url(pubkey.network));

    let account = accounts::get(&client, &pubkey.to_string()).await?;
    let nonce: u64 = account.speculative_nonce + 1;

    if account.balance.get_decimal() < amount.get_decimal() {
        return Ok(Response::InsufficientBalance(account.balance, amount));
    }
    // serialize payer
    let payer = PublicKey::from_str(&account.address)?;

    let mut txn = BlockchainTxnPaymentV1 {
        payee: payee.to_vec(),
        payer: payer.to_vec(),
        amount: u64::from(amount),
        nonce,
        fee: 0,
        signature: vec![],
    };
    txn.fee = txn
        .txn_fee(
            &get_txn_fees(&client)
                .await
                .map_err(|_| Error::getting_fees())?,
        )
        .map_err(|_| Error::getting_fees())?;

    print_proposed_txn(&txn)?;

    let adpu_cmd = txn.apdu_serialize(opts.account)?;

    let exchange_pay_tx_result = read_from_ledger(&ledger, adpu_cmd)?;

    if exchange_pay_tx_result.data.len() == 1 {
        return Ok(Response::UserDeniedTransaction);
    }

    let txn = BlockchainTxnPaymentV1::decode(exchange_pay_tx_result.data.as_slice())?;

    let envelope = txn.in_envelope();
    // submit the signed tansaction to the API
    let pending_txn_status = submit_txn(&client, &envelope).await?;

    Ok(Response::Txn(txn, pending_txn_status.hash, payer.network))
}

use std::str::FromStr;

impl FromStr for Payee {
    type Err = Box<dyn std::error::Error>;

    fn from_str(s: &str) -> std::result::Result<Self, Self::Err> {
        let pos = s
            .find('=')
            .ok_or_else(|| format!("invalid KEY=value: missing `=`  in `{}`", s))?;
        Ok(Payee {
            address: s[..pos].parse()?,
            amount: s[pos + 1..].parse()?,
        })
    }
}

pub fn print_proposed_txn(txn: &BlockchainTxnPaymentV1) -> Result {
    let payee = PublicKey::try_from(txn.payee.clone())?;
    let units = match payee.network {
        Network::TestNet => "TNT",
        Network::MainNet => "HNT",
    };

    let mut table = Table::new();
    println!("Creating the following transaction:");
    table.add_row(row!["Payee", &format!("Pay Amount {}", units), "Nonce", "DC Fee"]);
    table.add_row(row![payee, Hnt::from(txn.amount), txn.nonce, txn.fee]);
    table.printstd();
    println!(
        "WARNING: do not use this output as the source of truth. Instead, rely \
    on the Ledger Display"
    );
    Ok(())
}
