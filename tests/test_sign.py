#!/usr/bin/env python

from ledgerblue.comm import getDongle
import argparse
from binascii import unhexlify
import base58

parser = argparse.ArgumentParser()
parser.add_argument('--account_number', help="BIP32 account to retrieve. e.g. \"12345\".")
args = parser.parse_args()

if args.account_number == None:
    args.account_number = "12345"

derivation_path = [44, 501, int(args.account_number)]
derivation_path_hex = '{:02x}'.format(len(derivation_path)) + "".join('{:02x}'.format(x | 0x80000000) for x in derivation_path)

dongle = getDongle(True)

def sign_message(message):
    # Create APDU message.
    # CLA 0xE0
    # INS 0x03  SIGN_MESSAGE
    # P1 0x00   NO USER CONFIRMATION REQUIRED (0x01 otherwise)
    # P2 0x00   UNUSED
    message_hex = '{:04x}'.format(len(message) / 2) + message
    payload_hex = derivation_path_hex + message_hex
    apdu_hex = "E0030100" + '{:04x}'.format(len(payload_hex) / 2) + payload_hex
    apdu_bytes = bytearray.fromhex(apdu_hex)

    print("~~ Ledger Solana ~~")
    print("Sign Message")

    result = dongle.exchange(apdu_bytes)[0:64]

    print("Signature received: " + base58.b58encode(bytes(result)))

sign_message("deadbeef")
sign_message("5ca1ab1e")
# Longest currently successful message: 316 bytes
sign_message("325b55b142c74b1edcd4cfd8ce20d0783b29a9ab515cd27e4a3361fe0ad1ffb7c59145b89d3edf4dff4bdd2ada21948b9d756fe3d4ac6e0f189a3fbb4d631561413c70480d9127179049ed9a1c687d6dfa54a7011ade72aff541ed3990767426646ad76d085d1f490d86f2c650080644e63869b374e19c3e0cc01e81c16e8f1b7eaf89b96a2b395a47aeba053c7cb000ee9e47efbe51c6d0e15c613fa34a7e0d7027d34dd7d13f24ab74c1e032cb40301ecbc3a5403c765790c78c91357a8d1136fffac46da60b8d6fcee3884b275e7f412a63c25e8306f8f18b569dd2b1035c6cf3c3a59888aeb0edce08193f84f0d0fe50c2a10cab6c7ea90024280c83e0369322d1b8d497a71bf7f27b540acb249fa1c7bda8102a32a369cb02880c6a4f77415a4ed10e7ea76559415027029666e694fa3006a6d4f80fbc09c03e")
