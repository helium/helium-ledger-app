# Ledger Solana app

## Overview
This app is for a Nano S/X Ledger Wallet.
It currently does very little, and just exposes a minimal API (`get_app_config`, `get_address`). 

## Prerequisites

Install Vagrant and VirtualBox.

Download the recommended BOLOS SDK, GCC, and Clang and put them into directories
next to the one this README is in and with names that match what is in `Vagrantfile`.
Alternatively, use the included `prepare-devenv.sh` script, but at the time of this
writing, it downloads a version of GCC that is missing the `gcc` executable.

## Creating the development environment

To start the Ubuntu 18.04 VM:

```bash
$ vagrant up
```

To enter the VM:

```bash
$ vagrant ssh
```

Go to the app directory:

```bash
$ cd /vagrant
```

Set the `BOLOS_SDK` and `BOLOS_ENV` environment variables:

```bash
$ export BOLOS_SDK=/bolos-sdk
$ export BOLOS_ENV=/bolos-env
```

## Alternative Setup, For those not using Vagrant

To build and install the app on your Ledger Nano S you must set up the Ledger Nano S build environments. Please follow the Getting Started instructions at [here](https://ledger.readthedocs.io/en/latest/userspace/getting_started.html).

If you don't want to setup a global environnment, you can also setup one just for this app by sourcing `prepare-devenv.sh` with the right target (`s` or `x`).

install prerequisite and switch to a Nano X dev-env:

```bash
sudo apt install python3-venv python3-dev libudev-dev libusb-1.0-0-dev

# (x or s, depending on your device)
source prepare-devenv.sh x 
```

## Building and installing

Build and load the application

```
$ make load
```

Compile and load the app onto the device:
```bash
make load
```

Refresh the repo (required after Makefile edits):
```bash
make clean
```

Remove the app from the device:
```bash
make delete
```

## Troubleshooting:

If `make load` fails on `getDongle()`, open `/etc/udev/rules.d/20-hw1.rules` and
add: `OWNER="vagrant"` to each line.

Details are described in:
https://support.ledger.com/hc/en-us/articles/115005165269-Fix-connection-issues


## Example of Ledger wallet functionality

Test functionality:
```bash
# (x or s, depending on your device)
source prepare-devenv.sh x
python test_example.py --account_number 12345
```

## Documentation
This follows the specification available in the [`api.asc`](https://github.com/solana-labs/ledger-app-solana/blob/master/doc/api.asc).
In this project we'll create a Linux virtual machine capable of cross-compiling the
Ledger Wallet boilerplate application and then loading it onto Ledger Nano S.

