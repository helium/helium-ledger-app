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
    message_hex = '{:02x}'.format(len(message) / 2) + message
    payload_hex = derivation_path_hex + message_hex
    apdu_hex = "E0030100" + '{:02x}'.format(len(payload_hex) / 2) + payload_hex
    apdu_bytes = bytearray.fromhex(apdu_hex)

    print("~~ Ledger Solana ~~")
    print("Sign Message")

    result = dongle.exchange(apdu_bytes)[0:64]

    print("Signature received: " + base58.b58encode(bytes(result)))

sign_message("deadbeef")
sign_message("5ca1ab1e")
