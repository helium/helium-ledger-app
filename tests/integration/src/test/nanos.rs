use super::*;

#[tokio::test]
async fn test_pubkey() {
    let mut speculos = Speculos::new(Device::NanoS, 9000, 50000).await.unwrap();
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
    assert!(text.starts_with("Confirm Address"));
    println!("{text}");
    assert!(text.contains(&pubkey.to_string()[..10]));
    speculos.shutdown().await.unwrap();
}
