python3 -m venv ledger-env
ledger-env/bin/pip3 install ledgerblue
ledger-env/bin/python3 -m ledgerblue.loadApp $APP_LOAD_PARAMS_EVALUATED
rm -rf ledger-env app.hex
