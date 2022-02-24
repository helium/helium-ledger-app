use super::*;

#[tokio::test]
async fn test_pubkey() {
    let mut speculos = Speculos::new(Device::NanoX, 9000, 50000).await.unwrap();
    let transport = speculos.transport().await.unwrap();
    let pubkey = txns::get_pubkey(0, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();

    assert!(pubkey.network == Network::MainNet);
    let _pubkey = txns::get_pubkey(0, &transport, PubkeyDisplay::On)
        .await
        .unwrap();

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error getting events");
    assert!(text == "Confirm Address");

    //TODO: make pubkey show on screen
    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_payment() {
    let account = 0;
    let mut speculos = Speculos::new(Device::NanoX, 9001, 50001).await.unwrap();
    let transport = speculos.transport().await.unwrap();
    let payer = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let payee = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let amount = 50000;
    let memo = 31;
    let fee = 8888;
    let nonce = 7342;
    let payment = Payment {
        payee: payee.to_vec(),
        amount,
        memo,
    };

    let submitted_txn = BlockchainTxnPaymentV2 {
        payer: payer.to_vec(),
        payments: vec![payment],
        nonce,
        fee,
        signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Amount HNT .0005"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(4)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Recipient Address {}", payee.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Payment Memo HwAAAAAAAAA="
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Data Credit Fee 8888"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");

    let received_txn = BlockchainTxnPaymentV2::decode(response.data.as_slice())
        .expect("Failure to decode payment");

    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    payer
        .verify(&buf, &received_txn.signature)
        .expect("Error verifying signature");

    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_burn() {
    let account = 5;
    let mut speculos = Speculos::new(Device::NanoX, 9002, 50002).await.unwrap();
    let transport = speculos.transport().await.unwrap();
    let payer = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let payee = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let amount = 100000;
    let memo = 32;
    let fee = 8998;
    let nonce = 7342;

    let submitted_txn = BlockchainTxnTokenBurnV1 {
        payer: payer.to_vec(),
        payee: payee.to_vec(),
        amount,
        memo,
        nonce,
        fee,
        signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Burn HNT .001"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Recipient Address {}", payee.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Burn Memo lAAAAAAAAAA="
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Data Credit Fee {}", fee)
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");

    let received_txn = BlockchainTxnTokenBurnV1::decode(response.data.as_slice())
        .expect("Failure to decode payment");

    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    payer
        .verify(&buf, &received_txn.signature)
        .expect("Error verifying signature");

    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_sec_exchange() {
    let account = 255;
    let mut speculos = Speculos::new(Device::NanoX, 9003, 50003).await.unwrap();
    let transport = speculos.transport().await.unwrap();
    let payer = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let payee = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let amount = 100000000;
    let fee = 888998;
    let nonce = 7342;

    let submitted_txn = BlockchainTxnSecurityExchangeV1 {
        payer: payer.to_vec(),
        payee: payee.to_vec(),
        amount,
        nonce,
        fee,
        signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Amount HST 1"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Recipient Address {}", payee.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Data Credit Fee {}", fee)
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");

    let received_txn = BlockchainTxnSecurityExchangeV1::decode(response.data.as_slice())
        .expect("Failure to decode payment");

    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    payer
        .verify(&buf, &received_txn.signature)
        .expect("Error verifying signature");

    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_stake() {
    let account = 233;
    let mut speculos = Speculos::new(Device::NanoX, 9004, 50004).await.unwrap();
    let transport = speculos.transport().await.unwrap();
    let owner = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let address = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let stake = 1000000000000;
    let fee = 88891123;

    let submitted_txn = BlockchainTxnStakeValidatorV1 {
        owner: owner.to_vec(),
        address: address.to_vec(),
        stake,
        fee,
        owner_signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Stake HNT 10000"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Stake Address {}", address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Data Credit Fee {}", fee)
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");

    let received_txn = BlockchainTxnStakeValidatorV1::decode(response.data.as_slice())
        .expect("Failure to decode payment");

    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    owner
        .verify(&buf, &received_txn.owner_signature)
        .expect("Error verifying signature");

    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_unstake() {
    let account = 15;
    let mut speculos = Speculos::new(Device::NanoX, 9005, 50005).await.unwrap();
    let transport = speculos.transport().await.unwrap();
    let owner = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let address = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let stake_amount = 1000000000000;
    let fee = 88891123;
    let stake_release_height = 1_239_478;

    let submitted_txn = BlockchainTxnUnstakeValidatorV1 {
        owner: owner.to_vec(),
        address: address.to_vec(),
        stake_amount,
        stake_release_height,
        fee,
        owner_signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Unstake HNT 10000"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Stake Release Height {}", stake_release_height)
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Unstake Address {}", address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Data Credit Fee {}", fee)
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");
    let received_txn = BlockchainTxnUnstakeValidatorV1::decode(response.data.as_slice())
        .expect("Failure to decode payment");

    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    owner
        .verify(&buf, &received_txn.owner_signature)
        .expect("Error verifying signature");

    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_validator_self_transfer() {
    let account = 15;
    let mut speculos = Speculos::new(Device::NanoX, 9006, 50006).await.unwrap();
    let transport = speculos.transport().await.unwrap();
    let owner = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let old_address = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let new_address = PublicKey::from_str("14sf44Spo6t7Qs6FNhBttitR16n9ZJXppPgj1NoQPfD55vRK4i3")
        .expect("Error parsing pubkey");
    let stake_amount = 1000000000000;
    let payment_amount = 100000000;
    let fee = 88891123;

    let submitted_txn = BlockchainTxnTransferValidatorStakeV1 {
        new_owner: owner.to_vec(),
        old_owner: owner.to_vec(),
        new_address: new_address.to_vec(),
        old_address: old_address.to_vec(),
        fee,
        payment_amount,
        stake_amount,
        new_owner_signature: vec![],
        old_owner_signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Transfer HNT Stake 10000"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "HNT Paid to Old Owne 1"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(text.starts_with("Old Owner"));
    assert!(text.contains(&owner.to_string()[..12]));
    speculos.click_right().await.expect("Error clicking right");
    if text.contains("(1/2)") {
        let _text = speculos
            .collect_event_text(1)
            .await
            .expect("Error collecting events");
        speculos.click_right().await.expect("Error clicking right");
    }

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(text.starts_with("New Owner"));
    assert!(text.contains(&owner.to_string()[..12]));
    speculos.click_right().await.expect("Error clicking right");
    if text.contains("(1/2)") {
        let _text = speculos
            .collect_event_text(1)
            .await
            .expect("Error collecting events");
        speculos.click_right().await.expect("Error clicking right");
    }

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Old Address {}", old_address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("New Address {}", new_address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");
    let received_txn = BlockchainTxnTransferValidatorStakeV1::decode(response.data.as_slice())
        .expect("Failure to decode payment");
    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    owner
        .verify(&buf, &received_txn.old_owner_signature)
        .expect("Error verifying signature");
    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_validator_receive_transfer() {
    let account = 15;
    let mut speculos = Speculos::new(Device::NanoX, 9007, 50007).await.unwrap();

    let transport = speculos.transport().await.unwrap();
    let old_owner = txns::get_pubkey(6, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let new_owner = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();

    let old_address = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let new_address = PublicKey::from_str("14sf44Spo6t7Qs6FNhBttitR16n9ZJXppPgj1NoQPfD55vRK4i3")
        .expect("Error parsing pubkey");
    let stake_amount = 1000000000000;
    let payment_amount = 100000000;
    let fee = 88891123;

    let submitted_txn = BlockchainTxnTransferValidatorStakeV1 {
        new_owner: new_owner.to_vec(),
        old_owner: old_owner.to_vec(),
        new_address: new_address.to_vec(),
        old_address: old_address.to_vec(),
        fee,
        payment_amount,
        stake_amount,
        new_owner_signature: vec![],
        old_owner_signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Transfer HNT Stake 10000"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "HNT Paid to Old Owne 1"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(text.starts_with("Old Owner"));
    assert!(text.contains(&old_owner.to_string()[..12]));
    speculos.click_right().await.expect("Error clicking right");
    if text.contains("(1/2)") {
        let _text = speculos
            .collect_event_text(1)
            .await
            .expect("Error collecting events");
        speculos.click_right().await.expect("Error clicking right");
    }

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(text.starts_with("New Owner"));
    assert!(text.contains(&new_owner.to_string()[..12]));
    speculos.click_right().await.expect("Error clicking right");
    if text.contains("(1/2)") {
        let _text = speculos
            .collect_event_text(1)
            .await
            .expect("Error collecting events");
        speculos.click_right().await.expect("Error clicking right");
    }

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Old Address {}", old_address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("New Address {}", new_address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");
    let received_txn = BlockchainTxnTransferValidatorStakeV1::decode(response.data.as_slice())
        .expect("Failure to decode payment");
    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    new_owner
        .verify(&buf, &received_txn.new_owner_signature)
        .expect("Error verifying signature");
    speculos.shutdown().await.unwrap();
}

#[tokio::test]
async fn test_validator_send_transfer() {
    let account = 15;
    let mut speculos = Speculos::new(Device::NanoX, 9008, 50008).await.unwrap();

    let transport = speculos.transport().await.unwrap();
    let old_owner = txns::get_pubkey(account, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();
    let new_owner = txns::get_pubkey(6, &transport, PubkeyDisplay::Off)
        .await
        .unwrap();

    let old_address = PublicKey::from_str("145kQhY9Asu5ZxCpd1zifSkr2VYhDLWXQqgpQhgs8TEwA6SWWLa")
        .expect("Error parsing pubkey");
    let new_address = PublicKey::from_str("14sf44Spo6t7Qs6FNhBttitR16n9ZJXppPgj1NoQPfD55vRK4i3")
        .expect("Error parsing pubkey");
    let stake_amount = 1000000000000;
    let payment_amount = 100000000;
    let fee = 88891123;

    let submitted_txn = BlockchainTxnTransferValidatorStakeV1 {
        new_owner: new_owner.to_vec(),
        old_owner: old_owner.to_vec(),
        new_address: new_address.to_vec(),
        old_address: old_address.to_vec(),
        fee,
        payment_amount,
        stake_amount,
        new_owner_signature: vec![],
        old_owner_signature: vec![],
    };

    let adpu_cmd = submitted_txn.apdu_serialize(account).unwrap();
    let result_handle = tokio::spawn(async move { read_from_ledger(&transport, adpu_cmd).await });

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Transfer HNT Stake 10000"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(2)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "HNT Paid to Old Owne 1"
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(text.starts_with("Old Owner"));
    assert!(text.contains(&old_owner.to_string()[..12]));
    speculos.click_right().await.expect("Error clicking right");
    if text.contains("(1/2)") {
        let _text = speculos
            .collect_event_text(1)
            .await
            .expect("Error collecting events");
        speculos.click_right().await.expect("Error clicking right");
    }

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(text.starts_with("New Owner"));
    assert!(text.contains(&new_owner.to_string()[..12]));
    speculos.click_right().await.expect("Error clicking right");
    if text.contains("(1/2)") {
        let _text = speculos
            .collect_event_text(1)
            .await
            .expect("Error collecting events");
        speculos.click_right().await.expect("Error clicking right");
    }

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("Old Address {}", old_address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        &format!("New Address {}", new_address.to_string())
    ));
    speculos.click_right().await.expect("Error clicking right");

    let text = speculos
        .collect_event_text(1)
        .await
        .expect("Error collecting events");
    assert!(compare_ignoring_whitespace_and_capital_s(
        text,
        "Sign transaction? YES"
    ));
    speculos.click_both().await.expect("Error clicking both");

    let response = result_handle
        .await
        .expect("Error awaiting response")
        .expect("Error from Ledger interaction");
    let received_txn = BlockchainTxnTransferValidatorStakeV1::decode(response.data.as_slice())
        .expect("Failure to decode payment");
    let mut buf = vec![];
    submitted_txn.encode(&mut buf).unwrap();
    old_owner
        .verify(&buf, &received_txn.old_owner_signature)
        .expect("Error verifying signature");
    speculos.shutdown().await.unwrap();
}
