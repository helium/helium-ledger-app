#[macro_use]
extern crate prettytable;
mod ledger_api;
mod payment_txn;
mod pubkeybin;

use helium_api::Hnt;
use ledger_api::*;
use pubkeybin::{PubKeyBin, B58};
use qr2term::print_qr;
use std::process;
use structopt::StructOpt;
pub type Result<T = ()> = std::result::Result<T, Box<dyn std::error::Error>>;

static HELIUM_API_BASE_URL: &str = "https://api.helium.io/v1/";

#[derive(StructOpt, Debug)]
enum Units {
    /// Pay using Bones as units (must be integer)
    Bones { bones: u64 },
    /// Pay using HNT as units (up to 8 decimals are tolerated)
    Hnt { hnt: Hnt },
}

/// Interact with Ledger Nano S for hardware wallet management
#[derive(Debug, StructOpt)]
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
    Pay {
        /// Address of the payee
        address: String,
        /// Select HNT or Bones
        #[structopt(subcommand)]
        units: Units,
    },
}
fn main() {
    println!("Communicating with Ledger - follow prompts on screen");

    let cli = Cli::from_args();
    if let Err(e) = run(cli) {
        println!("error: {}", e);
        process::exit(1);
    }
}

fn run(cli: Cli) -> Result {
    match cli {
        Cli::Balance { qr_code } => {
            // get pubkey and display it on Ledger Screen
            let pubkey = ledger_api::get_pubkey(PubkeyDisplay::On)?;

            print_balance(&pubkey)?;
            if qr_code {
                print_qr(&pubkey.to_b58()?)?;
            }
            Ok(())
        }
        Cli::Pay { address, units } => {
            let amount = match units {
                Units::Hnt { hnt } => hnt,
                Units::Bones { bones } => Hnt::from_bones(bones),
            };

            println!("Creating transaction for:");
            println!("      {:0.*} HNT", 8, amount.get_decimal());
            println!("        =");
            println!("      {:} Bones", amount.to_bones());

            match ledger_api::pay(address, amount)? {
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

use helium_api::Client;
use prettytable::{format, Table};

fn print_balance(pubkey: &PubKeyBin) -> Result {
    let client = Client::new_with_base_url(HELIUM_API_BASE_URL.to_string());
    let address = pubkey.to_b58()?;
    let result = client.get_account(&address);

    let mut table = Table::new();
    table.set_format(*format::consts::FORMAT_NO_LINESEP_WITH_TITLE);
    table.set_titles(row![
        "Address",
        "Balance HNT",
        "Data Credits",
        "Security Tokens"
    ]);

    match result {
        Ok(account) => table.add_row(row![
            address,
            Hnt::from_bones(account.balance),
            account.dc_balance,
            account.sec_balance
        ]),
        Err(err) => table.add_row(row![address, H3 -> err.to_string()]),
    };

    table.printstd();
    Ok(())
}

use helium_api::BlockchainTxnPaymentV1;

pub fn print_txn(txn: &BlockchainTxnPaymentV1, hash: &str) -> Result<()> {
    let mut table = Table::new();
    table.add_row(row!["Payee", "Amount HNT", "Nonce", "Hash"]);

    let payee = PubKeyBin::copy_from_slice(txn.payee.as_slice()).to_b58()?;

    table.add_row(row![payee, Hnt::from_bones(txn.amount), txn.nonce, hash]);
    table.printstd();
    Ok(())
}
