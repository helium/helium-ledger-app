#[macro_use]
extern crate prettytable;

use helium_api::{pending_transactions::PendingTxnStatus, Hnt};
use helium_crypto::{public_key::PublicKey, Network};
use helium_proto::{BlockchainTxn, BlockchainTxnPaymentV1};
use ledger_api::*;
use qr2term::print_qr;
use std::{env, fmt, process};
use structopt::StructOpt;

mod ledger_api;

mod error;
pub use error::Error;
pub type Result<T = ()> = std::result::Result<T, Error>;

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
        /// Select account index to check balance for
        /// With scan option, you can display many balances
        #[structopt(long = "account", default_value = "0")]
        account: u8,
        /// Scans all accounts up until selected account index
        /// This is useful for displaying all balances
        #[structopt(long = "scan")]
        scan: bool,
    },
    /// Pay a given address.
    Pay {
        payee: Payee,
        /// Select account index to check balance
        #[structopt(long = "account", default_value = "0")]
        account: u8,
    },
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

pub(crate) struct Version {
    major: u8,
    minor: u8,
    revision: u8,
}

impl Version {
    pub(crate) fn from_bytes(bytes: [u8; 3]) -> Version {
        Version {
            major: bytes[0],
            minor: bytes[1],
            revision: bytes[2],
        }
    }
}

impl fmt::Display for Version {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "v{}.{}.{}", self.major, self.minor, self.revision)
    }
}

async fn run(cli: Cli) -> Result {
    let version = ledger_api::get_version()?;
    println!("Ledger running Helium {}", version);

    match cli {
        Cli::Balance {
            qr_code,
            account,
            scan,
        } => {
            if version.major < 2 && account != 0 {
                panic!("Upgrade your the Helium App to use additional wallet accounts");
            };

            if scan {
                if qr_code {
                    println!("WARNING: to output a QR Code, do not use scan")
                }
                let mut pubkeys = Vec::new();
                for i in 0..account {
                    pubkeys.push(ledger_api::get_pubkey(i, PubkeyDisplay::Off)?);
                }
                print_balance(&pubkeys).await?;
            } else {
                // get pubkey and display it on Ledger Screen
                let pubkey = ledger_api::get_pubkey(account, PubkeyDisplay::On)?;
                let output = pubkey.to_string();
                print_balance(&vec![pubkey]).await?;
                if qr_code {
                    print_qr(&output)?;
                }
            }

            Ok(())
        }
        Cli::Pay { payee, account } => {
            if version.major < 2 && account != 0 {
                panic!("Upgrade your the Helium App to use additional wallet accounts");
            };
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

            match ledger_api::pay(account, address, amount).await? {
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

async fn print_balance(pubkeys: &[PublicKey]) -> Result {
    // sample the first pubkey to determine network
    let network = pubkeys[0].network;

    let client = Client::new_with_base_url(api_url(network));
    let mut table = Table::new();
    table.set_format(*format::consts::FORMAT_NO_LINESEP_WITH_TITLE);
    let balance = match network {
        Network::TestNet => "Balance TNT",
        Network::MainNet => "Balance HNT",
    };

    if pubkeys.len() > 1 {
        table.set_titles(row![
            "Index",
            "Address",
            balance,
            "Data Credits",
            "Security Tokens"
        ]);
    } else {
        table.set_titles(row!["Address", balance, "Data Credits", "Security Tokens"]);
    }
    for (account_index, pubkey) in pubkeys.iter().enumerate() {
        let address = pubkey.to_string();
        let result = accounts::get(&client, &address).await;
        if pubkeys.len() > 1 {
            match result {
                Ok(account) => table.add_row(row![
                    account_index,
                    address,
                    account.balance,
                    account.dc_balance,
                    account.sec_balance
                ]),
                Err(err) => table.add_row(row![account_index, address, H3 -> err.to_string()]),
            };
        } else {
            match result {
                Ok(account) => table.add_row(row![
                    address,
                    account.balance,
                    account.dc_balance,
                    account.sec_balance
                ]),
                Err(err) => table.add_row(row![address, H3 -> err.to_string()]),
            };
        }
    }

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
