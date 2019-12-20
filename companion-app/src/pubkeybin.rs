use bs58;
use io::{Read, Write};
use std::io;

// Newtype to allow us to `impl Default` on a 33 element array.
#[derive(Clone, Copy)]
pub struct PubKeyBin(pub(crate) [u8; 33]);

impl PubKeyBin {
    pub fn copy_from_slice(data: &[u8]) -> Self {
        let mut result = PubKeyBin::default();
        result.0.copy_from_slice(&data);
        result
    }
}

impl Default for PubKeyBin {
    fn default() -> Self {
        PubKeyBin([0; 33])
    }
}

pub trait ReadWrite {
    fn read(reader: &mut dyn Read) -> std::io::Result<Self>
    where
        Self: std::marker::Sized;
    fn write(&self, writer: &mut dyn Write) -> std::io::Result<()>;
}

pub trait B58 {
    fn to_b58(&self) -> bs58::encode::Result<String>;
    fn from_b58(str: String) -> bs58::decode::Result<Self>
    where
        Self: std::marker::Sized;
}

impl B58 for PubKeyBin {
    fn to_b58(&self) -> bs58::encode::Result<String> {
        // First 0 value is the "version" number defined for addresses
        // in libp2p
        let mut data = [0u8; 34];
        data[1..].copy_from_slice(&self.0);
        Ok(bs58::encode(data.as_ref()).with_check().into_string())
    }

    fn from_b58(b58: String) -> bs58::decode::Result<Self> {
        // First 0 value is the version byte
        let data = bs58::decode(b58).with_check(Some(0)).into_vec()?;
        let mut pubkey_bin = PubKeyBin::default();
        pubkey_bin.0.copy_from_slice(&data[1..]);
        Ok(pubkey_bin)
    }
}

impl ToString for PubKeyBin {
    fn to_string(&self) -> String {
        self.to_b58().unwrap()
    }
}