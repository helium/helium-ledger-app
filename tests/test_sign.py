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
sign_message("176f03e95f86bd6748ad00bf8409a8c2804d09d3a16df1246631641c7a02f42da7ed92ea80c73a868cbf335c4781746c777d1d7c817773eeabbda4fd896a6f7a8058edfcc28d200e029ab4906d37d6e53f116c52d46a3b7499da5599099ec4353d9625ba4f48e8bf1e77b5512a5f1471d2d23bd44960eb8ddd476dc1b439acf8e6f66622aa5bab922fee0a28a5dc884407b49b3049768742c6c022687dcac968814dbe7f28ad91c68323067e67ebae173d09beaeac80e919e994fc7ee6fab2bcadb708db8de854c15d403389913f2f88e4f869f98882370e8481c25ca5be426e98add5a68a8c6de2e142031f6a7982998fedad7ecb61f6266a73834b323382c3cd11942e311a6c159fd24a87de02fa707fc86c506b2b44ab1d5cbce7aa3338eb5f38b238e8ee92b0d2e475ee")
