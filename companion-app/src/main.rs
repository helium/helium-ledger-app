#[macro_use]
extern crate prettytable;

mod hnt;
mod ledger_api;
mod pubkeybin;

use hnt::Hnt;
use ledger_api::*;
use pubkeybin::{PubKeyBin, B58};
use qr2term::print_qr;
use std::process;
use structopt::StructOpt;
pub type Result<T = ()> = std::result::Result<T, Box<dyn std::error::Error>>;

/// Create and manage Helium wallets
#[derive(Debug, StructOpt)]
enum Cli {
    /// Get wallet information
    Balance {
        /// Display QR code for a given single wallet.
        #[structopt(long = "qr")]
        qr_code: bool,
    },
    /// Pay a number of bones to a given address. Note that amount
    /// is parsed HNT by default and that 1 HNT is 100_000_000 bones
    Pay {
        /// Address of the payee
        address: String,

        /// Amount of HNT to transfer to payee
        #[structopt(name = "amount")]
        amount: Hnt,
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
        Cli::Pay { address, amount } => {
            println!("Creating transaction for:");
            println!("      {:0.*} HNT", 8, amount.get_decimal());
            println!("        =");
            println!("       {:} Bones", amount.to_bones());

            match ledger_api::pay(address, amount)? {
                PayResponse::Txn(txn) => print_txn(&txn),
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
    let client = Client::new();
    let address = pubkey.to_b58()?;
    let result = client.get_account(&address);

    let mut table = Table::new();
    table.set_format(*format::consts::FORMAT_NO_LINESEP_WITH_TITLE);
    table.set_titles(row![
        "Address",
        "Balance",
        "Data Credits",
        "Security Tokens"
    ]);

    match result {
        Ok(account) => table.add_row(row![
            address,
            account.balance,
            account.dc_balance,
            account.security_balance
        ]),
        Err(err) => table.add_row(row![address, H3 -> err.to_string()]),
    };

    table.printstd();
    Ok(())
}

use helium_proto::txn::TxnPaymentV1;
use prost::Message;
use sha2::{Digest, Sha256};

pub fn print_txn(txn: &TxnPaymentV1) {
    let mut txn_copy = txn.clone();
    // clear the signature so we can compute the hash
    txn_copy.signature = Vec::new();

    let mut hasher = Sha256::new();
    // write input message
    let mut buf = Vec::new();

    txn_copy.encode(&mut buf).unwrap();
    hasher.input(buf.as_slice());
    let result = hasher.result();

    let mut data = [0u8; 33];
    data[1..].copy_from_slice(&result);

    let mut table = Table::new();
    table.add_row(row!["Payee", "Amount", "Nonce", "Txn Hash"]);
    table.add_row(row![
        PubKeyBin::from_vec(&txn.payee).to_b58().unwrap(),
        txn.amount,
        txn.nonce,
        bs58::encode(data.as_ref()).with_check().into_string()
    ]);
    table.printstd();
}
