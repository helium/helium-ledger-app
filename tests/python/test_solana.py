from time import sleep
from ragger.backend.interface import BackendInterface, RAPDU, RaisePolicy

from .apps.solana import SolanaDerivationPath, SolanaClient, SystemInstructionTransfer, Message, verify_signature, ErrorType


def test_solana_simple_transfer(client, firmware):
    from_derivation_path = SolanaDerivationPath(12345)
    to_derivation_path = SolanaDerivationPath()

    sol = SolanaClient(client)
    from_public_key = sol.get_public_key(from_derivation_path)
    print("Transfer FROM public key :", from_public_key)
    to_public_key = sol.get_public_key(to_derivation_path)
    print("Transfer TO public key :", to_public_key)

    instruction: SystemInstructionTransfer = SystemInstructionTransfer(from_public_key, to_public_key, 42)
    print("SystemInstructionTransfer :", instruction)

    message: bytes = Message([instruction]).serialize()
    print("Message :", message)

    signature: bytes = sol.sign_message(from_derivation_path, message, True).data
    print("Received signature :", signature)

    verify_signature(from_public_key, message, signature)

    sleep(0.1)

def test_solana_simple_transfer_refused(client, firmware):
    from_derivation_path = SolanaDerivationPath(12345)
    to_derivation_path = SolanaDerivationPath()

    sol = SolanaClient(client)
    from_public_key = sol.get_public_key(from_derivation_path)
    print("Transfer FROM public key :", from_public_key)
    to_public_key = sol.get_public_key(to_derivation_path)
    print("Transfer TO public key :", to_public_key)

    instruction: SystemInstructionTransfer = SystemInstructionTransfer(from_public_key, to_public_key, 42)
    print("SystemInstructionTransfer :", instruction)

    message: bytes = Message([instruction]).serialize()
    print("Message :", message)

    client.raise_policy = RaisePolicy.RAISE_NOTHING
    rapdu: RAPDU = sol.sign_message(from_derivation_path, message, False)
    print("Received rapdu :", rapdu)
    assert rapdu.status == ErrorType.USER_CANCEL

    sleep(0.1)
