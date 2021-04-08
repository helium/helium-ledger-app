# Helium Application for Ledger

This is the official Helium wallet app for the Ledger Nano S. It is built for
the Blockain Open Ledger Operating System.

*This application has not been tested or validated on the Nano X.*

When installed on a Nano S, the app allows you to view your Helium address,
check your balance, and submit transactions while using the companion app, also
included in this repository.

## How to use Helium on Ledger

The most recent release of the Helium Ledger Companion App is available [here](https://github.com/helium/helium-ledger-app/releases). 

Please [follow instructions here](https://docs.helium.com/wallets/ledger) to learn how to use it!

# Development

```
BOLOS_SDK=~/nanos-secure-sdk make TESTNET=true load
```