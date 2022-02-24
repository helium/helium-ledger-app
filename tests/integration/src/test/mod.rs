mod nanos;
mod nanox;
pub mod speculos;

pub use speculos::*;
use std::str::FromStr;

use helium_crypto::{Network, PublicKey, Verify};
use helium_ledger::txns::{self, *};

fn compare_ignoring_whitespace_and_capital_s(s: String, other: &str) -> bool {
    let mut s: String = s.split_whitespace().collect();
    let mut other: String = other.to_string().split_whitespace().collect();

    // there is some bug where S is not emitted from the speculos events
    other.retain(|c| c != 'S');
    s.retain(|c| c != 'S');

    let ret = s == other;
    // print here for easier test failure debug
    if !ret {
        println!("{s} != {other}");
    }
    ret
}
