# Integration tests

## Generating Code for Unit Tests

`main.rs` (ie: `cargo run`) is a helper app that generates the function bodies
for unit tests. It uses existing transaction-to-ADPU implementations to 
generate the test payloads that the `save_context` functions would be 
processing. In addition, it generates the corresponding asserts for the unit
tests.

## Running Integration Tests

The integration tests leverage Rusts built-in unit testing framework. 
Therefore, only `cargo test` is needed to run the integration tests.

The test framework calls speculos to run the application in an emulated
environment where ADPU commands are communicated over TCP and button-presses
are implemented using the Rest API.

The tests are designed to be run as separate suites:

* `SPECULOS_DIR=~/speculos BUILD_DIR=../../bin/ cargo test test::nanos`
* `SPECULOS_DIR=~/speculos BUILD_DIR=../../bin/ cargo test test::nanox`

Each suite is designed such that its own tests may run concurrently. It is 
_not_ possible to run both suites (nanox and nanos) at the same time. The
suites creates a speculos instance for each individual test, binding each
one to a different set of ports different ports; the TCP port is used for
APDU commands and the REST API is used for button presses and reading of
events.

There are a few rough edges in the current integration testing:
* the character capital S (`S`) appears to be dropped in many of the events
fetched over the REST API. When running the emulator with the emulated screen,
however, the character is present
* the button presses on NanoX appear entirely non-functional. This is confirmed
when interacting with the app via the emulated screen and via the browser
interface