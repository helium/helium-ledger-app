use super::*;

#[derive(Debug, StructOpt)]
pub struct Cmd {
    /// Display QR code for a given single wallet.
    #[structopt(long = "qr")]
    pub qr_code: bool,
    /// Scans all accounts up until selected account index
    /// This is useful for displaying all balances
    #[structopt(long = "scan")]
    pub scan: bool,
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
        let ledger_transport = get_ledger_transport(&opts).await?;
        if self.scan {
            if self.qr_code {
                println!("WARNING: to output a QR Code, do not use scan")
            }
            let mut pubkeys = Vec::new();
            for i in 0..opts.account {
                pubkeys.push(super::get_pubkey(i, &ledger_transport, PubkeyDisplay::Off).await?);
            }
            print_balance(&pubkeys).await?;
        } else {
            let pubkey = super::get_pubkey( opts.account,&ledger_transport, PubkeyDisplay::On).await?;
            let output = pubkey.to_string();
            print_balance(&vec![pubkey]).await?;
            if self.qr_code {
                print_qr(&output)?;
            }
        }
        Ok(None)
    }
}



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
