#!/usr/bin/env python

from ledgerblue.comm import getDongle
import argparse
from binascii import unhexlify

parser = argparse.ArgumentParser()
parser.add_argument('--account_number', help="BIP32 account to retrieve. e.g. \"12345\".")
args = parser.parse_args()

if args.account_number == None:
	args.account_number = "12345"

account = '{:08x}'.format(int(args.account_number))

# Create APDU message.
# CLA 0xE0
# INS 0x02  GET_ADDRESS
# P1 0x01   USER CONFIRMATION REQUIRED (0x00 otherwise)
# P2 0x00   UNUSED
# Ask for confirmation
# txt = "E0020100" + '{:02x}'.format(len(donglePath) + 1) + '{:02x}'.format( int(len(donglePath) / 4 / 2)) + donglePath
# No confirmation
apduMessage = "E0020100" + '{:02x}'.format(len(account) + 1) + account
apdu = bytearray.fromhex(apduMessage)

print("~~ Ledger Boilerplate ~~")
print("Request Address")

dongle = getDongle(True)
result = dongle.exchange(apdu)

print("Address received: " + result.decode("utf-8"))
