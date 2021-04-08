#[macro_use]
extern crate prettytable;

use helium_api::{pending_transactions::PendingTxnStatus, Hnt};
use helium_crypto::{public_key::PublicKey, Network};
use helium_proto::{BlockchainTxn, BlockchainTxnPaymentV1};
use ledger_api::*;
use qr2term::print_qr;
use std::{env, process};
use structopt::StructOpt;

mod ledger_api;

pub type Result<T = ()> = std::result::Result<T, Box<dyn std::error::Error>>;

const DEFAULT_TESTNET_BASE_URL: &str = "https://testnet-api.helium.wtf/v1";

#[derive(StructOpt, Debug)]
enum Units {
    /// Pay using Bones as units (must be integer)
    Bones { bones: u64 },
    /// Pay using HNT as units (up to 8 decimals are tolerated)
    Hnt { hnt: Hnt },
}

/// Interact with Ledger Nano S for hardware wallet management
#[derive(Debug, StructOpt)]
#[allow(clippy::large_enum_variant)]
enum Cli {
    /// Get wallet information
    Balance {
        /// Display QR code for a given single wallet.
        #[structopt(long = "qr")]
        qr_code: bool,
    },
    /// Pay a given address.
    /// Use subcommand hnt or bones.
    /// Note that 1 HNT = 100,000,000 Bones = 100M Bones.
    Pay { payee: Payee },
}

#[derive(Debug)]
pub struct Payee {
    address: PublicKey,
    amount: Hnt,
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

#[tokio::main]
async fn main() {
    println!("Communicating with Ledger - follow prompts on screen");

    let cli = Cli::from_args();
    if let Err(e) = run(cli).await {
        println!("error: {}", e);
        process::exit(1);
    }
}

async fn run(cli: Cli) -> Result {
    match cli {
        Cli::Balance { qr_code } => {
            // get pubkey and display it on Ledger Screen
            let pubkey = ledger_api::get_pubkey(PubkeyDisplay::On)?;

            print_balance(&pubkey).await?;
            if qr_code {
                print_qr(&pubkey.to_string())?;
            }
            Ok(())
        }
        Cli::Pay { payee } => {
            let address = payee.address;
            let amount = payee.amount;
            println!("Creating transaction for:");
            let units = match address.network {
                Network::TestNet => "TNT",
                Network::MainNet => "HNT",
            };

            println!("      {:0.*} {}", 8, amount, units);
            println!("        =");
            println!("      {:} Bones", u64::from(amount));

            match ledger_api::pay(address, amount).await? {
                PayResponse::Txn(txn, hash) => print_txn(&txn, &hash).unwrap(),
                PayResponse::InsufficientBalance(balance, send_request) => {
                    println!(
                        "Account balance insufficient. {} Bones on account but attempting to send {}",
                        balance,
                        send_request,
                    );
                }
                PayResponse::UserDeniedTransaction => {
                    println!("Transaction not confirmed");
                }
            };
            Ok(())
        }
    }
}

use helium_api::{accounts, Client};
use prettytable::{format, Table};

async fn print_balance(pubkey: &PublicKey) -> Result {
    let client = Client::new_with_base_url(api_url(pubkey.network));
    let address = pubkey.to_string();
    let result = accounts::get(&client, &address).await;
    let mut table = Table::new();
    table.set_format(*format::consts::FORMAT_NO_LINESEP_WITH_TITLE);

    let balance = match pubkey.network {
        Network::TestNet => "Balance TNT",
        Network::MainNet => "Balance HNT",
    };

    table.set_titles(row!["Address", balance, "Data Credits", "Security Tokens"]);

    match result {
        Ok(account) => table.add_row(row![
            address,
            account.balance,
            account.dc_balance,
            account.sec_balance
        ]),
        Err(err) => table.add_row(row![address, H3 -> err.to_string()]),
    };

    table.printstd();
    Ok(())
}

use std::convert::TryFrom;

pub fn print_txn(txn: &BlockchainTxnPaymentV1, hash: &str) -> Result<()> {
    let mut table = Table::new();
    table.add_row(row!["Payee", "Amount HNT", "Nonce", "Hash"]);

    let payee = PublicKey::try_from(txn.payee.as_slice())?.to_string();

    table.add_row(row![payee, Hnt::from(txn.amount), txn.nonce, hash]);
    table.printstd();
    Ok(())
}

pub async fn submit_txn(client: &Client, txn: &BlockchainTxn) -> Result<PendingTxnStatus> {
    use helium_proto::Message;
    let mut data = vec![];
    txn.encode(&mut data)?;
    helium_api::pending_transactions::submit(client, &data)
        .await
        .map_err(|e| e.into())
}

fn api_url(network: Network) -> String {
    match network {
        Network::MainNet => {
            env::var("HELIUM_API_URL").unwrap_or_else(|_| helium_api::DEFAULT_BASE_URL.to_string())
        }
        Network::TestNet => env::var("HELIUM_TESTNET_API_URL")
            .unwrap_or_else(|_| DEFAULT_TESTNET_BASE_URL.to_string()),
    }
}
