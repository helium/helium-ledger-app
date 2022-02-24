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
