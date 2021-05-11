# Fuzzing

Building the fuzzer requires Clang and CMake.

To quickly get started fuzzing Solana message parserusing libFuzzer:

```shell
cd fuzzing
./build.sh
./run.sh
```

Initial corpus has been generated from the testcases found in the `libsol` directory.
