#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "instructions.h"
#include "register.h"
#include "types.h"
#include "cpu.h"

int main() {
    uint8_t b[] = {
        ADD, R0, R1,
        MOV, R2, R0,
        MOV, R3, R2,
        SUB, R3, R1,
        DIV, R3, R0,
        HLT
    };

    CPU cpu = new_cpu(b, sizeof(b));

    cpu.reg[0] = 3;
    cpu.reg[1] = 6;

    memcpy(cpu.mem, b, sizeof(b));

    run_cpu(&cpu);

    print_register(&cpu);
}