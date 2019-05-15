# Ledger boilerplate app

## Overview
This app is a boilerplate for a Nano S/X app.
It does very little, and just expose a minimal API (get_app_config, get_address). 

## Building and installing
To build and install the app on your Ledger Nano S you must set up the Ledger Nano S build environments. Please follow the Getting Started instructions at [here](https://ledger.readthedocs.io/en/latest/userspace/getting_started.html).

If you don't want to setup a global environnment, you can also setup one just for this app by sourcing `prepare-devenv.sh` with the right target (`s` or `x`).

install prerequisite and switch to a Nano X dev-env:

```bash
sudo apt install python3-venv
source prepre-devenv.sh x
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


## Example of Ledger wallet functionality

Test functionality:
```bash
source prepre-devenv.sh x (or s, depending on your device)
python test_exemple.py --account_number 12345
```

## Documentation
This follows the specification available in the [`api.asc`](https://github.com/ledgerHQ/ledger-app-boilerplate/doc/api.asc).
