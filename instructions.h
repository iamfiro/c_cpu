#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

enum instr {
    MOV = 0x01,
    ADD = 0x02,
    SUB = 0x03,
    MUL = 0x04,
    DIV = 0x05,
    JMP = 0x06,
    JZ = 0x07,
    JNZ = 0x08,
    HLT = 0x09,
};

#endif // INSTRUCTIONS_H