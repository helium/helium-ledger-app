#!/bin/bash
set -euo pipefail

TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

(  
  cd "$TMPDIR"

  cat <<'EOF' > app.hex
$APP_CODE
EOF
  
  cat <<'EOF' > requirements.txt
$APP_REQUIREMENTS
EOF
  
  python3 -m venv ledger-env
  ledger-env/bin/pip3 install --require-hashes -r requirements.txt
  ledger-env/bin/python3 -m ledgerblue.loadApp $APP_LOAD_PARAMS_EVALUATED
)
  



