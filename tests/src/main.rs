use solana_remote_wallet::ledger::LedgerWallet;
use solana_remote_wallet::remote_wallet::{
    initialize_wallet_manager, DerivationPath, RemoteWallet,
};
use solana_sdk::{message::Message, pubkey::Pubkey, system_instruction};
use solana_stake_program::stake_instruction;
use std::collections::HashSet;
use std::sync::Arc;

fn get_ledger() -> (Arc<LedgerWallet>, Pubkey) {
    let wallet_manager = initialize_wallet_manager().expect("Couldn't start wallet manager");

    // Update device list
    const NO_DEVICE_HELP: &str = "No Ledger found, make sure you have a unlocked Ledger connected with the Ledger Wallet Solana running";
    wallet_manager.update_devices().expect(NO_DEVICE_HELP);
    assert!(wallet_manager.list_devices().len() > 0, NO_DEVICE_HELP);

    // Fetch the base pubkey of a connected ledger device
    let ledger_base_pubkey = wallet_manager
        .list_devices()
        .iter()
        .filter(|d| d.manufacturer == "ledger".to_string())
        .nth(0)
        .map(|d| d.pubkey.clone())
        .expect("No ledger device detected");

    let ledger = wallet_manager
        .get_ledger(&ledger_base_pubkey)
        .expect("get device");

    (ledger, ledger_base_pubkey)
}

fn test_ledger_pubkey() {
    let (ledger, ledger_base_pubkey) = get_ledger();

    let mut pubkey_set = HashSet::new();
    pubkey_set.insert(ledger_base_pubkey);

    let pubkey_0_0 = ledger
        .get_pubkey(
            &DerivationPath {
                account: Some(0),
                change: Some(0),
            },
            false,
        )
        .expect("get pubkey");
    pubkey_set.insert(pubkey_0_0);
    let pubkey_0_1 = ledger
        .get_pubkey(
            &DerivationPath {
                account: Some(0),
                change: Some(1),
            },
            false,
        )
        .expect("get pubkey");
    pubkey_set.insert(pubkey_0_1);
    let pubkey_1 = ledger
        .get_pubkey(
            &DerivationPath {
                account: Some(1),
                change: None,
            },
            false,
        )
        .expect("get pubkey");
    pubkey_set.insert(pubkey_1);
    let pubkey_1_0 = ledger
        .get_pubkey(
            &DerivationPath {
                account: Some(1),
                change: Some(0),
            },
            false,
        )
        .expect("get pubkey");
    pubkey_set.insert(pubkey_1_0);

    assert_eq!(pubkey_set.len(), 5); // Ensure keys at various derivation paths are unique
}

/// This test requires interactive approval of message signing on the ledger.
fn test_ledger_sign_garbage() {
    let (ledger, _ledger_base_pubkey) = get_ledger();

    let derivation_path = DerivationPath {
        account: Some(12345),
        change: None,
    };

    // Test invalid message
    let data = hex::decode("deadbeef").expect("decode hex");
    ledger.sign_message(&derivation_path, &data).unwrap_err();
}

// This test requires interactive approval of message signing on the ledger.
fn test_ledger_sign_transaction() {
    let (ledger, ledger_base_pubkey) = get_ledger();

    let derivation_path = DerivationPath {
        account: Some(12345),
        change: None,
    };

    let from = ledger
        .get_pubkey(&derivation_path, false)
        .expect("get pubkey");
    let instruction = system_instruction::transfer(&from, &ledger_base_pubkey, 42);
    let message = Message::new(vec![instruction]).serialize();
    let signature = ledger
        .sign_message(&derivation_path, &message)
        .expect("sign transaction");
    assert!(signature.verify(&from.as_ref(), &message));

    // Test large transaction
    let recipients: Vec<(Pubkey, u64)> = (0..10).map(|_| (Pubkey::new_rand(), 42)).collect();
    let instructions = system_instruction::transfer_many(&from, &recipients);
    let message = Message::new(instructions).serialize();
    let signature = ledger
        .sign_message(&derivation_path, &message)
        .expect("sign transaction");
    assert!(signature.verify(&from.as_ref(), &message));
}

fn test_ledger_sign_transaction_too_big() {
    // Test too big of a transaction
    let (ledger, _ledger_base_pubkey) = get_ledger();

    let derivation_path = DerivationPath {
        account: Some(12345),
        change: None,
    };

    let from = ledger
        .get_pubkey(&derivation_path, false)
        .expect("get pubkey");
    let recipients: Vec<(Pubkey, u64)> = (0..100).map(|_| (Pubkey::new_rand(), 42)).collect();
    let instructions = system_instruction::transfer_many(&from, &recipients);
    let message = Message::new(instructions).serialize();
    ledger.sign_message(&derivation_path, &message).unwrap_err();
}

/// This test requires interactive approval of message signing on the ledger.
fn test_ledger_delegate_stake() {
    let (ledger, ledger_base_pubkey) = get_ledger();

    let derivation_path = DerivationPath {
        account: Some(12345),
        change: None,
    };

    let authorized_pubkey = ledger
        .get_pubkey(&derivation_path, false)
        .expect("get pubkey");
    let stake_pubkey = ledger_base_pubkey;
    let vote_pubkey = Pubkey::default();
    let instruction =
        stake_instruction::delegate_stake(&stake_pubkey, &authorized_pubkey, &vote_pubkey);
    let message = Message::new(vec![instruction]).serialize();
    let signature = ledger
        .sign_message(&derivation_path, &message)
        .expect("sign transaction");
    assert!(signature.verify(&authorized_pubkey.as_ref(), &message));
}

/// This test requires interactive approval of message signing on the ledger.
fn test_ledger_advance_nonce_account() {
    let (ledger, _ledger_base_pubkey) = get_ledger();

    let derivation_path = DerivationPath {
        account: Some(12345),
        change: None,
    };

    let authorized_pubkey = ledger
        .get_pubkey(&derivation_path, false)
        .expect("get pubkey");
    let nonce_account = Pubkey::new(&[1u8; 32]);
    let instruction =
        system_instruction::advance_nonce_account(&nonce_account, &authorized_pubkey);
    let message = Message::new(vec![instruction]).serialize();
    let signature = ledger
        .sign_message(&derivation_path, &message)
        .expect("sign transaction");
    assert!(signature.verify(&authorized_pubkey.as_ref(), &message));
}

/// This test requires interactive approval of message signing on the ledger.
fn test_ledger_advance_nonce_account_separate_fee_payer() {
    let (ledger, _ledger_base_pubkey) = get_ledger();

    let derivation_path = DerivationPath {
        account: Some(12345),
        change: None,
    };

    let authorized_pubkey = ledger
        .get_pubkey(&derivation_path, false)
        .expect("get pubkey");
    let nonce_account = Pubkey::new(&[1u8; 32]);
    let fee_payer = Pubkey::new(&[2u8; 32]);
    let instruction =
        system_instruction::advance_nonce_account(&nonce_account, &authorized_pubkey);
    let message = Message::new_with_payer(vec![instruction], Some(&fee_payer)).serialize();
    let signature = ledger
        .sign_message(&derivation_path, &message)
        .expect("sign transaction");
    assert!(signature.verify(&authorized_pubkey.as_ref(), &message));
}

fn main() {
    test_ledger_pubkey();
    test_ledger_sign_garbage();
    test_ledger_sign_transaction();
    test_ledger_sign_transaction_too_big();
    test_ledger_delegate_stake();
    test_ledger_advance_nonce_account();
    test_ledger_advance_nonce_account_separate_fee_payer();
}
