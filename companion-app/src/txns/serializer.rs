use super::*;

pub const INS_GET_VERSION: u8 = 0x01;
pub const INS_GET_PUBLIC_KEY: u8 = 0x02;
pub const INS_SIGN_PAYMENT_TXN: u8 = 0x08;
pub const INS_SIGN_VALIDATOR_STAKE_TXN: u8 = 0x09;
pub const INS_SIGN_VALIDATOR_TXFER_TXN: u8 = 0x0A;
pub const INS_SIGN_VALIDATOR_UNSTAKE_TXN: u8 = 0x0B;

pub trait ApduSerializer {
    fn apdu_serialize(&self, account: u8) -> Result<APDUCommand>;
}

pub struct VersionRequest;

impl ApduSerializer for VersionRequest {
    fn apdu_serialize(&self, _account: u8) -> Result<APDUCommand> {
        Ok(APDUCommand {
            cla: 0xe0,
            ins: INS_GET_VERSION,
            p1: 0x00,
            p2: 0x00,
            data: vec![],
        })
    }
}

pub struct PubkeyRequest {
    pub display: PubkeyDisplay,
}

impl ApduSerializer for PubkeyRequest {
    fn apdu_serialize(&self, account: u8) -> Result<APDUCommand> {
        Ok(APDUCommand {
            cla: 0xe0,
            ins: INS_GET_PUBLIC_KEY,
            p1: self.display as u8,
            p2: account,
            data: vec![],
        })
    }
}

impl ApduSerializer for BlockchainTxnPaymentV1 {
    fn apdu_serialize(&self, account: u8) -> Result<APDUCommand> {
        let mut data = Vec::new();
        data.write_u64::<LE>(self.amount)?;
        data.write_u64::<LE>(self.fee)?;
        data.write_u64::<LE>(self.nonce)?;
        data.push(0);
        data.extend(self.payee.clone());

        Ok(APDUCommand {
            cla: 0xe0,
            ins: INS_SIGN_PAYMENT_TXN,
            p1: account,
            p2: 0x00,
            data,
        })
    }
}

impl ApduSerializer for BlockchainTxnStakeValidatorV1 {
    fn apdu_serialize(&self, account: u8) -> Result<APDUCommand> {
        let mut data: Vec<u8> = Vec::new();
        data.write_u64::<LE>(self.stake)?;
        data.write_u64::<LE>(self.fee)?;
        data.push(0);
        data.extend(self.address.clone());

        Ok(APDUCommand {
            cla: 0xe0,
            ins: INS_SIGN_VALIDATOR_STAKE_TXN,
            p1: account,
            p2: 0x00,
            data,
        })
    }
}

impl ApduSerializer for BlockchainTxnTransferValidatorStakeV1 {
    fn apdu_serialize(&self, account: u8) -> Result<APDUCommand> {
        let mut data: Vec<u8> = Vec::new();

        data.write_u64::<LE>(self.stake_amount)?;
        data.write_u64::<LE>(self.payment_amount)?;
        data.write_u64::<LE>(self.fee)?;

        data.push(0);
        data.extend(self.new_owner.clone());

        data.push(0);
        data.extend(self.old_owner.clone());

        data.push(0);
        data.extend(self.new_address.clone());

        data.push(0);
        data.extend(self.old_address.clone());

        Ok(APDUCommand {
            cla: 0xe0,
            ins: INS_SIGN_VALIDATOR_TXFER_TXN,
            p1: account,
            p2: 0x00,
            data,
        })
    }
}
impl ApduSerializer for BlockchainTxnUnstakeValidatorV1 {
    fn apdu_serialize(&self, account: u8) -> Result<APDUCommand> {
        let mut data: Vec<u8> = Vec::new();
        data.write_u64::<LE>(self.stake_amount)?;
        data.write_u64::<LE>(self.stake_release_height)?;
        data.write_u64::<LE>(self.fee)?;
        data.push(0);
        data.extend(self.address.clone());

        Ok(APDUCommand {
            cla: 0xe0,
            ins: INS_SIGN_VALIDATOR_UNSTAKE_TXN,
            p1: account,
            p2: 0x00,
            data,
        })
    }
}
