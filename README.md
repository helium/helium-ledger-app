# ledger-app-helium

This is the official Helium wallet app for the Ledger Nano S.

When installed on a Nano S, the app allows you to view your Helium address,
check your balance, and submit transactions.

No binaries are provided at this time. To build and install the Helium app on
your Ledger Nano S, follow Ledger's [setup instructions](https://ledger.readthedocs.io/en/latest/userspace/getting_started.html).

## Security Model

The attack surface for using the Helium wallet app on a Ledger Nano S comprises
the Helium app itself, the system firmware running on the Nano S, the computer
that the Nano S is connected to, and posession/control of the device. For our
purposes, the app only needs to ensure its own correctness and protect the
user from the computer that the Nano S is connected to. Other attack surfaces
are beyond our control; we assume that the user physically controls the
device, is not running malicious/buggy software on the device, and follows
proper security protocols. The goal of the Helium app is to achieve perfect
security given these assumptions.

The main attack vector that we are concerned with, then, is a computer running
malicious sofware. This software may imitate programs like `helium-wallet` in such
a way that the user cannot tell the difference, but secretly act maliciously.
Specifically, the computer can do the following:

1. Lie to the user about which actions it is performing. *Example: the user
   runs `./helium-wallet -l info` to display their public key to so that they
   may receive payment; yet a hard-coded address is displayed
2. Lie to the user about who the recipient is. *Example: the user
   runs `./helium-wallet -l pay IntendedAddress amount`, yeet the program again
   uses a hard-coded address

To combat these attacks, apps must make use of the embedded display on the
Nano S. If data sent to/from the Nano S is displayed on the screen, the user
can verify that the computer is not lying about what it sent or received. In
the interest of user-friendliness, we would like to display as little
information as much as possible, but each omission brings with it the risk of
introducing a vulnerability. Therefore, an app should display all data by
default, and omit data only after subjecting the omission to extreme scrutiny.
The Sia app adheres to this principle more closely than most Ledger apps, and
as a result is not affected by certain vulnerabilities affecting those apps.
