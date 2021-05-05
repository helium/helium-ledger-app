# Helium Application for Ledger

This is the official Helium wallet app for the Ledger Nano S. It is built for
the Blockain Open Ledger Operating System.

*This application has not been tested or validated on the Nano X.*

When installed on a Nano S, the app allows you to view your Helium address,
check your balance, and submit transactions while using the companion app, also
included in this repository.

The Helium Ledger App uses bip32 path `44'/904'/n'/0'/0'` for Mainnet and 
`44'/905'/n'/0'/0'` for Testnet.

## How to use Helium on Ledger

The most recent release of the Helium Ledger Companion App is available 
[here](https://github.com/helium/helium-ledger-app/releases). 

Please [follow instructions here](https://docs.helium.com/wallets/ledger) to learn how to use it!

# Development

You can follows the instructions [here](https://ledger.readthedocs.io/en/0/nanos/setup.html#first-app-hello-world
) from Ledger docs

Another way is to download the BOLOS_SDK for which you are compiling. For example, for Nano X, 
clone the repo into your home directory

```
git clone git@github.com:LedgerHQ/nanos-secure-sdk.git
```

From this helium-ledger repo, you can now build and load the app for the testnet in the following
way:

```
BOLOS_SDK=~/nanos-secure-sdk make TESTNET=true load
```

The load will fail unless you are on the app selection screen.

## Toolchains: GCC and Clang

Nano S is fairly easy to compile for, but if you are compiling for the Nano X, you need to be a bit more particular
about the toolchains:
* gcc-arm-none-eabi-7-2017-q4-major-linux
* clang9.0.0 plus the matching ld.lld (eg: cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS=clang;lld \
  -DCMAKE_INSTALL_PREFIX=/usr/local ../llvm-project/llvm)

## Emulator: speculos

You can test the app in an emulator by installing [`speculos`](https://github.com/LedgerHQ/speculos). This is in fact
the only way to test the app for Nano X.

Note that `speculos` is particular about the version of construct used and pip install actually brings too new of a 
version in.

To fix:
```
pip3 uninstall construct
pip3 install construct=2.9.45
```

This is the _only_ way to test the app for Nano X. Once installed, you'll want to run something like this:
```
./speculos.py  --model nanox  ~/helium-ledger-app/bin/app.elf
```

## Emulator: usage

The emulator provides a TCP IP socket at `127.0.0.1:9999`. You can use 
[`ledgerwallet`](https://speculos.ledger.com/user/clients.html) or you can use the companion app included in this 
project; just use the flag `--emulator`.

The TCP socket protocol is rather straightforward; prepend the ADPU serialization with the length of said serialization.
It is a 32-bit integer in big-endian.
