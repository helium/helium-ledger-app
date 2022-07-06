use helium_ledger;
use helium_ledger::txns::*;
use std::str::FromStr;

mod util;
use util::*;

type Result<T = ()> = std::result::Result<T, Error>;

fn main() -> Result {
    test_save_payment_context(
        8,
        PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")?,
        8765432,
        1234,
        231,
        34243,
    )?;
    test_save_burn_context(
        8,
        PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")?,
        9993121,
        13234,
        21231,
        342443,
    )?;
    test_save_transfer_sec_context(
        8,
        PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")?,
        9219,
        1331234,
        3424343,
    )?;
    test_save_validator_stake_context(
        8,
        PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")?,
        9122219,
        11234,
    )?;
    test_save_validator_transfer_context(
        8,
        PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")?,
        PublicKey::from_str("14sf44Spo6t7Qs6FNhBttitR16n9ZJXppPgj1NoQPfD55vRK4i3")?,

        PublicKey::from_str("11YzguYUdqCzonqrqCSMEcZPA2YGNdu9JQBQMbkwcErvUrixy4R")?,
        PublicKey::from_str("11u76hY8bzrbo7AdJ68qEkXDGPNP3CpUdt42Lgar9YSJ99KyYNS")?,
        9122219,
        11234,
        9123219,

    )?;
    test_save_validator_unstake_context(
        8,
        PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")?,
        912114299,
        9103312219,

        771234,
    )?;

    Ok(())
}

fn test_save_payment_context(
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
    println!("uint8_t payee[] = {};", array_in_c(&payee_bytes));
    println!("{}", apdu.into_save_context_call("payment")?);
    println!(
        "\
    assert(ctx.amount == {amount});\r\n\
    assert(ctx.fee == {fee});\r\n\
    assert(global.account_index == {account_index});\r\n\
    assert(ctx.memo == {memo});\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.payee[i] == payee[i]);\r\n\
    }}\r\n\
    "
    );
    Ok(())
}

fn test_save_burn_context(
    account_index: u8,
    payee: PublicKey,
    amount: u64,
    memo: u64,
    nonce: u64,
    fee: u64,
) -> Result {
    let burn = BlockchainTxnTokenBurnV1 {
        payer: vec![],
        payee: payee.to_vec(),
        amount,
        memo,
        nonce,
        fee,
        signature: vec![],
    };
    let apdu = burn.apdu_serialize(account_index)?;
    let payee_bytes = pubkey_to_c_array(&payee);
    println!("uint8_t payee[] = {};", array_in_c(&payee_bytes));
    println!("{}", apdu.into_save_context_call("burn")?);
    println!(
        "\
    assert(ctx.amount == {amount});\r\n\
    assert(ctx.fee == {fee});\r\n\
    assert(global.account_index == {account_index});\r\n\
    assert(ctx.memo == {memo});\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.payee[i] == payee[i]);\r\n\
    }}\r\n\
    "
    );
    Ok(())
}

fn test_save_transfer_sec_context(
    account_index: u8,
    payee: PublicKey,
    amount: u64,
    nonce: u64,
    fee: u64,
) -> Result {
    let transfer_sec = BlockchainTxnSecurityExchangeV1 {
        payer: vec![],
        payee: payee.to_vec(),
        amount,
        nonce,
        fee,
        signature: vec![],
    };
    let apdu = transfer_sec.apdu_serialize(account_index)?;
    let payee_bytes = pubkey_to_c_array(&payee);
    println!("uint8_t payee[] = {};", array_in_c(&payee_bytes));
    println!("{}", apdu.into_save_context_call("transfer_sec")?);
    println!(
        "\
    assert(ctx.amount == {amount});\r\n\
    assert(ctx.fee == {fee});\r\n\
    assert(global.account_index == {account_index});\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.payee[i] == payee[i]);\r\n\
    }}\r\n\
    "
    );
    Ok(())
}

fn test_save_validator_stake_context(
    account_index: u8,
    address: PublicKey,
    stake: u64,
    fee: u64,
) -> Result {
    let transfer_sec = BlockchainTxnStakeValidatorV1 {
        owner: vec![],
        address: address.to_vec(),
        stake,
        fee,
        owner_signature: vec![],
    };
    let apdu = transfer_sec.apdu_serialize(account_index)?;
    let address_bytes = pubkey_to_c_array(&address);
    println!("uint8_t address[] = {};", array_in_c(&address_bytes));
    println!("{}", apdu.into_save_context_call("stake_validator")?);
    println!(
        "\
    assert(ctx.stake == {stake});\r\n\
    assert(ctx.fee == {fee});\r\n\
    assert(global.account_index == {account_index});\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.address[i] == address[i]);\r\n\
    }}\r\n\
    "
    );
    Ok(())
}

fn test_save_validator_transfer_context(
    account_index: u8,
    new_owner: PublicKey,
    old_owner: PublicKey,
    new_address: PublicKey,
    old_address: PublicKey,
    stake_amount: u64,
    payment_amount: u64,
    fee: u64,
) -> Result {
    let transfer_sec = BlockchainTxnTransferValidatorStakeV1 {
        old_owner: old_owner.to_vec(),
        new_owner: new_owner.to_vec(),
        old_address: old_address.to_vec(),
        new_address: new_address.to_vec(),
        stake_amount,
        payment_amount,
        fee,
        old_owner_signature: vec![],
        new_owner_signature: vec![],

    };
    let apdu = transfer_sec.apdu_serialize(account_index)?;
    println!("uint8_t old_owner[] = {};", array_in_c(&pubkey_to_c_array(&old_owner)));
    println!("uint8_t new_owner[] = {};", array_in_c(&pubkey_to_c_array(&new_owner)));
    println!("uint8_t old_address[] = {};", array_in_c(&pubkey_to_c_array(&old_address)));
    println!("uint8_t new_address[] = {};", array_in_c(&pubkey_to_c_array(&new_address)));
    println!("{}", apdu.into_save_context_call("transfer_validator")?);
    println!(
        "\
    assert(ctx.stake_amount == {stake_amount});\r\n\
    assert(ctx.payment_amount == {payment_amount});\r\n\
    assert(ctx.fee == {fee});\r\n\
    assert(global.account_index == {account_index});\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.new_owner[i] == new_owner[i]);\r\n\
    }}\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.old_owner[i] == old_owner[i]);\r\n\
    }}\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.new_address[i] == new_address[i]);\r\n\
    }}\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.old_address[i] == old_address[i]);\r\n\
    }}\r\n\
    "
    );
    Ok(())
}

fn test_save_validator_unstake_context(
    account_index: u8,
    address: PublicKey,
    stake_amount: u64,
    stake_release_height: u64,
    fee: u64,
) -> Result {
    let transfer_sec = BlockchainTxnUnstakeValidatorV1 {
        owner: vec![],
        address: address.to_vec(),
        fee,
        stake_amount,
        stake_release_height,
        owner_signature: vec![],
    };
    let apdu = transfer_sec.apdu_serialize(account_index)?;
    let address_bytes = pubkey_to_c_array(&address);
    println!("uint8_t address[] = {};", array_in_c(&address_bytes));
    println!("{}", apdu.into_save_context_call("unstake_validator")?);
    println!(
        "\
    assert(ctx.stake_amount == {stake_amount});\r\n\
    assert(ctx.stake_release_height == {stake_release_height});\r\n\
    assert(ctx.fee == {fee});\r\n\
    assert(global.account_index == {account_index});\r\n\
    for(uint8_t i=0; i<34; i++) {{\r\n\
    \tassert(ctx.address[i] == address[i]);\r\n\
    }}\r\n\
    "
    );
    Ok(())
}


