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

The most recent release of the Helium Ledger Companion App is available [here](https://github.com/helium/helium-ledger-app/releases). 

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