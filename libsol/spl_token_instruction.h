#pragma once

#include "spl/token.h"

#define SplTokenBody(b) Token_ ## b ## _Body
#define SplTokenKind(b) b

#define SplTokenInstructionKind Token_TokenInstruction_Tag

typedef struct SplTokenMultisigners {
    const Pubkey* first;
    uint8_t count;
} SplTokenMultisigners;

typedef enum {
    SplTokenSignKindSingle,
    SplTokenSignKindMulti,
} SplTokenSignKind;

typedef struct {
    SplTokenSignKind kind;
    union {
        struct {
            const Pubkey* signer;
        } single;
        struct {
            const Pubkey* account;
            SplTokenMultisigners signers;
        } multi;
    };
} SplTokenSign;

extern const Pubkey spl_token_program_id;

typedef struct SplTokenInitializeMintInfo {
    const Pubkey* mint_account;
    const Pubkey* token_account;
    const Pubkey* owner;
    SplTokenBody(InitializeMint) body;
} SplTokenInitializeMintInfo;

typedef struct SplTokenInitializeAccountInfo {
    const Pubkey* token_account;
    const Pubkey* mint_account;
    const Pubkey* owner;
} SplTokenInitializeAccountInfo;

typedef struct SplTokenInitializeMultisigInfo {
    const Pubkey* multisig_account;
    SplTokenMultisigners signers;
    SplTokenBody(InitializeMultisig) body;
} SplTokenInitializeMultisigInfo;

typedef struct SplTokenTransferInfo {
    const Pubkey* src_account;
    const Pubkey* dest_account;
    SplTokenSign sign;
    SplTokenBody(Transfer) body;
} SplTokenTransferInfo;

typedef struct SplTokenApproveInfo {
    const Pubkey* token_account;
    const Pubkey* delegate;
    SplTokenSign sign;
    SplTokenBody(Approve) body;
} SplTokenApproveInfo;

typedef struct SplTokenRevokeInfo {
    const Pubkey* token_account;
    SplTokenSign sign;
} SplTokenRevokeInfo;

typedef struct SplTokenSetOwnerInfo {
    const Pubkey* token_account;
    const Pubkey* new_owner;
    SplTokenSign sign;
} SplTokenSetOwnerInfo;

typedef struct SplTokenMintToInfo {
    const Pubkey* mint_account;
    const Pubkey* token_account;
    SplTokenSign sign;
    SplTokenBody(MintTo) body;
} SplTokenMintToInfo;

typedef struct SplTokenBurnInfo {
    const Pubkey* token_account;
    SplTokenSign sign;
    SplTokenBody(Burn) body;
} SplTokenBurnInfo;

typedef struct SplTokenCloseAccountInfo {
    const Pubkey* token_account;
    const Pubkey* dest_account;
    SplTokenSign sign;
} SplTokenCloseAccountInfo;

typedef struct SplTokenInfo {
    SplTokenInstructionKind kind;
    union {
        SplTokenInitializeMintInfo initialize_mint;
        SplTokenInitializeAccountInfo initialize_account;
        SplTokenInitializeMultisigInfo initialize_multisig;
        SplTokenTransferInfo transfer;
        SplTokenApproveInfo approve;
        SplTokenRevokeInfo revoke;
        SplTokenSetOwnerInfo set_owner;
        SplTokenMintToInfo mint_to;
        SplTokenBurnInfo burn;
        SplTokenCloseAccountInfo close_account;
    };
} SplTokenInfo;

int parse_spl_token_instructions(
    const Instruction* instruction,
    const MessageHeader* header,
    SplTokenInfo* info
);
int print_spl_token_info(
    const SplTokenInfo* info,
    const MessageHeader* header
);
