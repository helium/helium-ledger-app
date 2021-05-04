use crate::{Network, Opts, Result, StructOpt, Version};

mod stake;
mod transfer;
mod unstake;

#[derive(Debug, StructOpt)]
/// Commands for validators
pub enum Cmd {
    /// Stake a validator with the given wallet as the owner.
    Stake(stake::Cmd),
    /// Transfer a validator stake to a new validator and/or owner
    Transfer(Box<transfer::Cmd>),
    /// Unstake a validator with the given wallet as the owner.
    Unstake(unstake::Cmd),
}

impl Cmd {
    pub(crate) async fn run(
        self,
        opts: Opts,
        version: Version,
    ) -> Result<Option<(String, Network)>> {
        if version.major < 2 {
            panic!("Upgrade the Helium Ledger App to use validator commands");
        };
        match self {
            Cmd::Stake(stake) => stake.run(opts, version).await,
            Cmd::Transfer(transfer) => transfer.run(opts, version).await,
            Cmd::Unstake(unstake) => unstake.run(opts, version).await,
        }
    }

}
