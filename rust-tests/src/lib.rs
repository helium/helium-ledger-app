#[cfg(test)]
mod tests {
    use serial_test_derive::serial;
    use solana_remote_wallet::remote_wallet::{
        initialize_wallet_manager, DerivationPath, RemoteWallet,
    };
    use solana_sdk::{message::Message, pubkey::Pubkey, system_instruction};
    use std::collections::HashSet;

    #[test]
    #[serial]
    fn ledger_pubkey_test() {
        let wallet_manager = initialize_wallet_manager();

        // Update device list
        wallet_manager.update_devices().expect("No Ledger found, make sure you have a unlocked Ledger connected with the Ledger Wallet Solana running");
        assert!(wallet_manager.list_devices().len() > 0);

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

        let mut pubkey_set = HashSet::new();
        pubkey_set.insert(ledger_base_pubkey);

        let pubkey_0_0 = ledger
            .get_pubkey(&DerivationPath {
                account: 0,
                change: Some(0),
            })
            .expect("get pubkey");
        pubkey_set.insert(pubkey_0_0);
        let pubkey_0_1 = ledger
            .get_pubkey(&DerivationPath {
                account: 0,
                change: Some(1),
            })
            .expect("get pubkey");
        pubkey_set.insert(pubkey_0_1);
        let pubkey_1 = ledger
            .get_pubkey(&DerivationPath {
                account: 1,
                change: None,
            })
            .expect("get pubkey");
        pubkey_set.insert(pubkey_1);
        let pubkey_1_0 = ledger
            .get_pubkey(&DerivationPath {
                account: 1,
                change: Some(0),
            })
            .expect("get pubkey");
        pubkey_set.insert(pubkey_1_0);

        assert_eq!(pubkey_set.len(), 5); // Ensure keys at various derivation paths are unique
    }

    /// This test requires interactive approval of message signing on the ledger.
    #[test]
    #[serial]
    fn ledger_sign_transaction_test() {
        let wallet_manager = initialize_wallet_manager();

        // Update device list
        wallet_manager.update_devices().expect("No Ledger found, make sure you have a unlocked Ledger connected with the Ledger Wallet Solana running");
        assert!(wallet_manager.list_devices().len() > 0);

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

        let derivation_path = DerivationPath {
            account: 12345,
            change: None,
        };

        let from = ledger.get_pubkey(&derivation_path).expect("get pubkey");
        let instruction = system_instruction::transfer(&from, &ledger_base_pubkey, 42);
        let message = Message::new(vec![instruction]).serialize();
        let signature = ledger
            .sign_message(&derivation_path, &message)
            .expect("sign transaction");
        assert!(signature.verify(&from.as_ref(), &message));

        // Test large transaction
        let recipients: Vec<(Pubkey, u64)> = (0..4).map(|_| (Pubkey::new_rand(), 42)).collect();
        let instructions = system_instruction::transfer_many(&from, &recipients);
        let message = Message::new(instructions).serialize();
        let signature = ledger
            .sign_message(&derivation_path, &message)
            .expect("sign transaction");
        assert!(signature.verify(&from.as_ref(), &message));

        // Test hex string message
        let data = hex::decode("5ca1ab1e").expect("decode hex");

        let signature = ledger
            .sign_message(&derivation_path, &data)
            .expect("send apdu");
        assert!(signature.verify(&from.as_ref(), &data));
    }
}
