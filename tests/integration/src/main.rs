use helium_ledger;
use helium_ledger::txns::*;
use std::str::FromStr;

mod util;
use util::*;

type Result<T = ()> = std::result::Result<T, Error>;

fn main() -> Result {
    test_save_context(
        8,
        PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")?,
        8765432,
        1234,
        231,
        34243,
    )?;
    Ok(())
}

fn test_save_context(
    account_index: u8,
    payee: PublicKey,
    amount: u64,
    memo: u64,
    nonce: u64,
    fee: u64,
) -> Result {
    let payment = BlockchainTxnPaymentV2 {
        payer: vec![],
        payments: vec![Payment {
            payee: payee.to_vec(),
            amount,
            memo,
        }],
        nonce,
        fee,
        signature: vec![],
    };
    let apdu = payment.apdu_serialize(account_index)?;
    let payee_bytes = pubkey_to_c_array(&payee);
    println!("uint8_t payee[] = {}", array_in_c(&payee_bytes));
    println!("{}", apdu.into_save_context_call("payment")?);
    println!(
        "\
    assert(ctx.amount == {amount});\r\n\
    assert(ctx.fee == {fee});\r\n\
    assert(ctx.account_index == {account_index});\r\n\
    assert(ctx.memo == {memo});\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.payee[i] == payee[i]);\r\n\
    }}\r\n\
    "
    );
    Ok(())
}

