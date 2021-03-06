# Ethereum Bytecode Disassembler

## Decompiles EVM bytecode to Opcode Mnemonics

# Building
```
make
```

# Running
```
./bin/ebd
```

### Sample
```
./ebd --input 600160020100 --output sample

sample:
-----------
PUSH1 0x01
PUSH1 0x02
ADD
STOP
```

# Testing
```
./bin/tests/main_t
```

## License

[AGPL-3.0-only](https://github.com/abrandec/vision_evm/blob/master/LICENSE)

## Disclaimer

_This software is being provided as is. No guarantee, representation or warranty is being made, express or implied, as to the safety or correctness of the program. They have not been audited and as such there can be no assurance they will work as intended, and users may experience delays, failures, errors, omissions or loss of transmitted information. The creators are not liable for any of the foregoing. Users should proceed with caution and use at their own risk._