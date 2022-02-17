use super::*;

pub trait IntoSaveContextCall {
    fn into_save_context_call(&self, label: &str) -> Result<String>;
}

pub fn pubkey_to_c_array(pubkey: &PublicKey) -> Vec<u8> {
    let mut bytes = vec![0];
    bytes.extend(pubkey.to_vec());
    bytes
}

pub fn array_in_c(data: &Vec<u8>) -> String {
    let mut arr = String::new();
    for el in data {
        let num = el.to_string();
        arr.push_str(&format!("{num}, "));
    }
    format!("{{{arr}}}")
}

impl IntoSaveContextCall for APDUCommand {
    fn into_save_context_call(&self, label: &str) -> Result<String> {
        let arr = array_in_c(&self.data);
        Ok(format!(
            "\
            {label}Context_t ctx;\r\n\
            uint8_t {label}_buffer[] = {arr};\r\n\
            save_{label}_context({}, {}, {label}_buffer, {}, &ctx);",
            self.p1,
            self.p2,
            self.data.len()
        ))
    }
}

use thiserror::Error;

#[derive(Error, Debug)]
pub enum Error {
    #[error("Helium Crypto error: {0}")]
    Crypto(#[from] helium_crypto::error::Error),
    #[error("Helium Ledger error: {0}")]
    HeliumLedger(#[from] helium_ledger::Error),
}
