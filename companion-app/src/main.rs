#[macro_use]
extern crate prettytable;

use helium_api::{pending_transactions::PendingTxnStatus, Hnt};
use helium_crypto::Network;
use helium_proto::BlockchainTxn;
use qr2term::print_qr;
use std::{env, fmt, process};
use structopt::StructOpt;

mod error;
mod txns;

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

/// Common options for most wallet commands
#[derive(Debug, StructOpt)]
pub struct Opts {
    /// File(s) to use
    #[structopt(short = "a", long = "account", default_value = "0")]

    /// Select account index to stake from
    #[structopt(long = "account", default_value = "0")]
    pub account: u8,
}

#[derive(Debug, StructOpt)]
pub struct Cli {
    #[structopt(flatten)]
    opts: Opts,

    #[structopt(flatten)]
    cmd: Cmd,
}

/// Interact with Ledger Nano S for hardware wallet management
#[derive(Debug, StructOpt)]
#[allow(clippy::large_enum_variant)]
enum Cmd {
    /// Get wallet information
    Balance(txns::balance::Cmd),
    /// Pay a given address.
    Pay(txns::pay::Cmd),
    /// Stake a validator
    Validator(txns::validator::Cmd),
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
    let version = txns::get_app_version()?;
    println!("Ledger running Helium App {}\r\n", version);

    let result = match cli.cmd {
        Cmd::Balance(balance) => balance.run(cli.opts, version).await?,
        Cmd::Pay(pay) => pay.run(cli.opts, version).await?,
        Cmd::Validator(validator) => validator.run(cli.opts, version).await?,
    };
    if let Some((hash, network)) = result {
        println!("\nSuccessfully submitted transaction to API:");

        let mut table = Table::new();
        table.add_row(row!["Network", "Hash"]);
        table.add_row(row![network, hash]);
        table.printstd();

        println!("To check on transaction status, monitor the following URL:");
        println!("     {}/pending_transactions/{}", api_url(network), hash);
    }

    Ok(())
}

use helium_api::Client;
use prettytable::{format, Table};

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
