[![Build Status](https://travis-ci.org/solana-labs/ledger-app-solana.svg?branch=master)](https://travis-ci.org/solana-labs/ledger-app-solana)

# Solana app for Ledger Wallet

## Overview

This app adds support for the Solana native token to Ledger Nano S hardware wallet.

Current Features:
- Pubkey queries
- Parse, display and sign all Solana CLI generated transaction formats
- Blind sign arbitrary transactions (Enabled via settings)

## Prerequisites
### For building the app
* [Install Docker](https://docs.docker.com/get-docker/)
* For Linux hosts, install the Ledger Nano [udev rules](https://github.com/LedgerHQ/udev-rules)
#### Build the [Ledger App Builder](https://developers.ledger.com/docs/nano-app/build/) Docker image
1. Clone the git repository
```
git clone https://github.com/LedgerHQ/ledger-app-builder.git
```
1. Change directories
```
cd ledger-app-builder
```
1. Checkout the target commit
```
git checkout 73c9e07
```
1. Build the image
```
docker build -t ledger-app-builder:73c9e07 .
```
  * If permissions errors are encountered, ensure that your user is in the `docker`
group and that the session has been restarted

### For working with the device
* Install Python3 PIP
Ubuntu Linux:
```
sudo apt install pip3
```
MacOS
```
brew install python3
```
* Install ledgerblue python module
```
pip3 install ledgerblue
```

### For running the test suite
* [Rust](https://rustup.rs/)
* Solana [system dependencies](https://github.com/solana-labs/solana/#1-install-rustc-cargo-and-rustfmt)

## Build
It is highly recommended that you read and understand the [Ledger App Builder](https://developers.ledger.com/docs/nano-app/build/)
build process before proceeding.  A convenience wrapper script (`./docker-make`) has been provided for simplicity

```bash
./docker-make
```
### Clean
```bash
./docker-make clean
```

## Working with the device
Requires that the `BOLOS_SDK` envvar [be set](https://developers.ledger.com/docs/nano-app/load/)
### Load
```bash
make load-only
```

### Delete
```bash
make delete
```

## Test
### Unit
Run C tests:
```bash
make -C libsol
```
### Integration
First enable `blind-signing` in the App settings
```bash
cargo run --manifest-path tests/Cargo.toml
```
