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

derivationPath = "8000002C800001F5"
account = '{:08x}'.format(int(args.account_number) | 0x80000000)

# Create APDU message.
# CLA 0xE0
# INS 0x02  GET_PUBKEY
# P1 0x01   USER CONFIRMATION REQUIRED (0x00 otherwise)
# P2 0x00   UNUSED
# Ask for confirmation
# txt = "E0020100" + '{:02x}'.format(len(donglePath) + 1) + '{:02x}'.format( int(len(donglePath) / 4 / 2)) + donglePath
# No confirmation
apduMessage = "E0020000" + '{:02x}'.format(len(derivationPath + account)/2 + 1) + '{:02x}'.format(len(derivationPath + account)/8) + derivationPath + account
apdu = bytearray.fromhex(apduMessage)

print("~~ Ledger Solana ~~")
print("Request Pubkey")

dongle = getDongle(True)
result = dongle.exchange(apdu)[0:32]

print("Pubkey received: " + base58.b58encode(bytes(result)))
