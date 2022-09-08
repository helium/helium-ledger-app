from contextlib import contextmanager
from typing import Optional, List, Generator
from enum import IntEnum
import base58
from nacl.signing import VerifyKey
from ragger.backend.interface import BackendInterface, RAPDU

from ..utils import bip32_harden

INS_GET_PUBKEY = 0x05
INS_SIGN_MESSAGE = 0x06

P1_NON_CONFIRM = 0x00
P1_CONFIRM = 0x01

P2_EXTEND = 0x01
P2_MORE = 0x02


PUBLIC_KEY_LENGTH = 32

MAX_CHUNK_SIZE = 255


STATUS_OK = 0x9000

class ErrorType:
    NO_APP_RESPONSE                 = 0x6700
    SDK_EXCEPTION                   = 0x6801
    SDK_INVALID_PARAMETER           = 0x6802
    SDK_EXCEPTION_OVERFLOW          = 0x6803
    SDK_EXCEPTION_SECURITY          = 0x6804
    SDK_INVALID_CRC                 = 0x6805
    SDK_INVALID_CHECKSUM            = 0x6806
    SDK_INVALID_COUNTER             = 0x6807
    SDK_NOT_SUPPORTED               = 0x6808
    SDK_INVALID_STATE               = 0x6809
    SDK_TIMEOUT                     = 0x6810
    SDK_EXCEPTION_PIC               = 0x6811
    SDK_EXCEPTION_APP_EXIT          = 0x6812
    SDK_EXCEPTION_IO_OVERFLOW       = 0x6813
    SDK_EXCEPTION_IO_HEADER         = 0x6814
    SDK_EXCEPTION_IO_STATE          = 0x6815
    SDK_EXCEPTION_IO_RESET          = 0x6816
    SDK_EXCEPTION_CX_PORT           = 0x6817
    SDK_EXCEPTION_SYSTEM            = 0x6818
    SDK_NOT_ENOUGH_SPACE            = 0x6819
    NO_APDU_RECEIVED                = 0x6982
    USER_CANCEL                     = 0x6985
    SOLANA_INVALID_MESSAGE          = 0x6a80
    SOLANA_SUMMARY_FINALIZE_FAILED  = 0x6f00
    SOLANA_SUMMARY_UPDATE_FAILED    = 0x6f01
    UNIMPLEMENTED_INSTRUCTION       = 0x6d00
    INVALID_CLA                     = 0x6e00

class SolanaDerivationPath:
    as_bytes: bytes

    def __init__(self, account: Optional[int] = None, change: Optional[int] = None):
        length = 2
        if account is not None:
            length += 1
            if change is not None:
                length += 1

        self.as_bytes: bytes = (length).to_bytes(1, byteorder='big')
        self.as_bytes += (bip32_harden(44)).to_bytes(4, byteorder='big')
        self.as_bytes += (bip32_harden(501)).to_bytes(4, byteorder='big')

        if account != None:
            self.as_bytes += (bip32_harden(account)).to_bytes(4, byteorder='big')
            if change != None:
                self.as_bytes += (bip32_harden(change)).to_bytes(4, byteorder='big')

class SystemInstruction(IntEnum):
    CreateAccount           = 0x00
    Assign                  = 0x01
    Transfer                = 0x02
    CreateAccountWithSeed   = 0x03
    AdvanceNonceAccount     = 0x04
    WithdrawNonceAccount    = 0x05
    InitializeNonceAccount  = 0x06
    AuthorizeNonceAccount   = 0x07
    Allocate                = 0x08
    AllocateWithSeed        = 0x09
    AssignWithSeed          = 0x10
    TransferWithSeed        = 0x11
    UpgradeNonceAccount     = 0x12

PROGRAM_ID_SYSTEM = "11111111111111111111111111111111"

# Fake blockhash so this example doesn't need a network connection. It should be queried from the cluster in normal use.
FAKE_RECENT_BLOCKHASH = "11111111111111111111111111111111"

class MessageHeader:
    def __init__(self, num_required_signatures: int, num_readonly_signed_accounts: int, num_readonly_unsigned_accounts: int):
        self.num_required_signatures = num_required_signatures
        self.num_readonly_signed_accounts = num_readonly_signed_accounts
        self.num_readonly_unsigned_accounts = num_readonly_unsigned_accounts

    def serialize(self) -> bytes:
        return self.num_required_signatures.to_bytes(1, byteorder='little') + \
               self.num_readonly_signed_accounts.to_bytes(1, byteorder='little') + \
               self.num_readonly_unsigned_accounts.to_bytes(1, byteorder='little')

class AccountMeta:
    pubkey: bytes
    is_signer: bool
    is_writable: bool
    def __init__(self, pubkey: bytes, is_signer: bool, is_writable: bool):
        self.pubkey = pubkey
        self.is_signer = is_signer
        self.is_writable = is_writable

# Only support Transfer instruction for now
# TODO add other instructions if the need arises
class Instruction:
    program_id: bytes
    accounts: List[AccountMeta]
    data: bytes
    from_pubkey: bytes
    to_pubkey: bytes

class SystemInstructionTransfer(Instruction):
    def __init__(self, from_pubkey: bytes, to_pubkey: bytes, amount: int):
        self.from_pubkey = from_pubkey
        self.to_pubkey = to_pubkey
        self.program_id = base58.b58decode(PROGRAM_ID_SYSTEM)
        self.accounts = [AccountMeta(from_pubkey, True, True), AccountMeta(to_pubkey, False, True)]
        self.data = (SystemInstruction.Transfer).to_bytes(4, byteorder='little') + (amount).to_bytes(8, byteorder='little')

# Cheat as we only support 1 SystemInstructionTransfer currently
# TODO add support for multiple transfers and other instructions if the needs arises
class CompiledInstruction:
    program_id_index: int
    accounts: List[int]
    data: bytes
    def __init__(self, program_id_index: int, accounts: List[int], data: bytes):
        self.program_id_index = program_id_index
        self.accounts = accounts
        self.data = data

    def serialize(self) -> bytes:
        serialized: bytes = self.program_id_index.to_bytes(1, byteorder='little')
        serialized += len(self.accounts).to_bytes(1, byteorder='little')
        for account in self.accounts:
            serialized += (account).to_bytes(1, byteorder='little')
        serialized += len(self.data).to_bytes(1, byteorder='little')
        serialized += self.data
        return serialized

class Message:
    def __init__(self, instructions: List[Instruction]):
        # Cheat as we only support 1 SystemInstructionTransfer currently
        # TODO add support for multiple transfers and other instructions if the needs arises
        self.header = MessageHeader(2, 0, 1)
        self.account_keys = [instructions[0].to_pubkey, instructions[0].from_pubkey, instructions[0].program_id]
        self.recent_blockhash = base58.b58decode(FAKE_RECENT_BLOCKHASH)
        self.compiled_instructions = [CompiledInstruction(2, [1, 0], instructions[0].data)]

    def serialize(self) -> bytes:
        serialized: bytes = self.header.serialize()
        serialized += len(self.account_keys).to_bytes(1, byteorder='little')
        for account_key in self.account_keys:
            serialized += account_key
        serialized += self.recent_blockhash
        serialized += len(self.compiled_instructions).to_bytes(1, byteorder='little')
        serialized += self.compiled_instructions[0].serialize()
        return serialized

def extend_and_serialize_multiple_derivations_paths(derivations_paths: List[SolanaDerivationPath]):
    serialized: bytes = len(derivations_paths).to_bytes(1, byteorder='little')
    for derivations_path in derivations_paths:
        serialized += derivations_path.as_bytes
    return serialized

def verify_signature(from_public_key: bytes, message: bytes, signature: bytes):
    assert len(signature) == 64, "signature size incorrect"
    verify_key = VerifyKey(from_public_key)
    verify_key.verify(message, signature)


# from .apps.solana_builder import SystemInstruction


CLA = 0xE0

class SolanaClient:
    client: BackendInterface

    def __init__(self, client):
        self._client = client

    def get_public_key(self, derivation_path : SolanaDerivationPath) -> bytes:
        public_key:RAPDU = self._client.exchange(CLA, INS_GET_PUBKEY, P1_NON_CONFIRM, P2_MORE, derivation_path.as_bytes)
        assert len(public_key.data) == PUBLIC_KEY_LENGTH, "'from' public key size incorrect"
        return public_key.data

    def sign_message(self, derivation_path : SolanaDerivationPath, message: bytes, accept: bool) -> RAPDU:
        assert len(message) <= 65535, "Message to sign is too long"

        header: bytes = extend_and_serialize_multiple_derivations_paths([derivation_path])

        # Check to see if this data needs to be split up and sent in chunks.
        max_size = MAX_CHUNK_SIZE - len(header)
        message_splited = [message[x:x+max_size] for x in range(0, len(message), max_size)]
        # Add the header to every chunk
        message_splited_prefixed = [header + s for s in message_splited]

        # Send all chunks with P2_MORE except for the last chunk, and P2_EXTEND if we send more than one chunk
        if len(message_splited_prefixed) > 1:
            final_p2 = P2_EXTEND
            for m in message_splited_prefixed[:-1]:
                self._client.exchange(CLA, INS_SIGN_MESSAGE, P1_CONFIRM, P2_MORE | P2_EXTEND, m)
        else:
            final_p2 = 0

        # response = self._exchange(INS_SIGN_MESSAGE, P1_CONFIRM, final_p2, message_splited_prefixed[-1])
        with self._client.exchange_async(CLA, INS_SIGN_MESSAGE, P1_CONFIRM, final_p2, message_splited_prefixed[-1]):
            for _ in range(3):
                self._client.right_click()
            if not accept:
                self._client.right_click()
            self._client.both_click()
        response = self._client.last_async_response

        # Last received apdu contains the signature
        # assert len(response.data) == 64, "signature size incorrect"
        # return response.data
        return response
