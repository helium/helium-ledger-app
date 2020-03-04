# Companion Application

This is the companion application to be run on a laptop or desktop running Linux, Mac, or Windows. It submits the parameters of your requests to the Ledger Nano S, which will then display the information for confirmation.

It is important to use the Ledger Nano S as the source of truth when confirming transactions as it is much more secure than your laptop or desktop and a compromised version of this application could be running.

# Features

This is a command line tool that allow you to do two primary operations:

* view your public key on the Ledger Nano S and check balance on your laptop/desktop

* submit transactions to the Ledger Nano S for signing, which are then submitted to the blockchain by this application once approved

# Usage

At any time, you can run the binary with no parameters to get a description of options. To do any of the commands, make sure your Ledger Nano S is plugged in, unlocked, and that the Helium app is running with the prompt: *"Waiting for commands"*.

Here is a quick reference for what you can do:

## Check Your Public Key and Balance

```
helium-ledger-app balance
```

Your Nano S will display the public key, which you should double-check that it matches the output of your screen. The Nano S screen is the source of truth and a compromised companion app could display an alternate address.

Click both buttons on the Ledger Nano S at once to exit the screen.

You can optionally display a QR code by using the `helium-ledger-app balance --qr`

## Submit a Transaction

```
helium-ledger-app pay <RECIPIENT> <AMOUNT>
```

`<RECIPENT>` needs to be replaced with a base58 encoded Helium address and the intended `<AMOUNT>` should be given as an HNT value with or without decimals. Note that the actual type used on the blockchain is a unsigned 64-bit integer and that unit is a Bone. 1 HNT = 10e8 Bones = 100_000_000

After you press enter, the request will be submitted to the Ledger Nano S which will display these values for confirmation. Press both buttons on the Ledger Nano S at the same time to proceed through the screens and on the transaction confirmation screen, click on the right button.

Note that, currently, transactions are free so the Data Credit Fee is 0. 

Once again, be careful in verifying that the details on the Ledger Nano S match what you input; the source of truth is always what is displayed on the Ledger screen.
