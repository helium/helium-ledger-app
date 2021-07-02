# Helium Application for Ledger

This is the official Helium wallet app for Leger hardware wallets. It is built for
Ledger's Blockchain Open Ledger Operating System (BOLOS).

When installed, the app allows you to view your Helium address, check your
balance, and submit transactions while using companion apps, such as the 
[helium-ledger-cli](https://github.com/helium/helium-ledger-cli)

The Helium Ledger App uses bip32 path `44'/904'/n'/0'/0'` for MainNet and
`44'/905'/n'/0'/0'` for TestNet.

## How to use Helium on Ledger

The most recent release of the Helium Ledger CLI App is available
[here](https://github.com/helium/helium-ledger-cli/releases).

Please [follow instructions here](https://docs.helium.com/wallets/ledger) to
learn how to use it!

# Development

Pull the latest version of the builder Docker container.

```
docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
```

From this repo, you can now build the app for the testnet in the following
way:

```
docker run \
    --rm \
    -v "$(realpath .):/app" \
  ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest \
    make TESTNET=true clean all
```

Note: The above command builds a testnet version of the app. You can choose to
remove the `TESTNET=true` environment variable for a mainnet build.

## Emulator: speculos

You can test the app in an emulator also in the speculos Docker image.

```
docker pull ghcr.io/ledgerhq/speculos:latest
```

You can now run the emulator by passing in the built app.

```
docker run \
    --rm \
    -it \
    -v $(realpath bin):/apps \
    --publish 40000:40000 \
  ghcr.io/ledgerhq/speculos:latest \
    --model nanos \
    --seed secret \
    --display text \
    --apdu-port 40000 \
    /apps/app.elf
```

The emulator provides a TCP IP socket at `127.0.0.1:40000`. You can use
[`ledgerwallet`](https://speculos.ledger.com/user/clients.html) or you can use
[helium-ledger-cli](https://github.com/helium/helium-ledger-cli) by specifying
a port for the emulator: `--emulator 40000`.

The TCP socket protocol is rather straightforward; prepend the ADPU
serialization with the length of said serialization. It is a 32-bit big-endian
integer.
