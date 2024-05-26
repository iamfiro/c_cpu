#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "instructions.h"

#define MEMORY_SIZE 256

typedef struct {
    i8 ZF:1; // 제로 플래그
    i8 CF:1; // 캐리 플래그
    i8 AF:1; // 보조 캐리
    i8 OF:1; // 오버플로우 플래그
    i8 SF:1; // 사인 플래그
    i8 PF:1; // 패리티 플래그
} FlagsRegister;

typedef struct {
    i32 eax;
    i32 ebx;
    i32 ecx;
    i32 edx;
} GeneralPurposeRegisters;

typedef struct {
    uint8_t mem[MEMORY_SIZE];
    GeneralPurposeRegisters gpr;
    FlagsRegister flags;
    i8 PC;
    i8 MAR;
    i8 MBR;
    i8 IR;
    i8 reg[8];
} CPU;

void fetchCycle(CPU *c);
void ALU(CPU *c);
void increment_pc(CPU *c, int value);

CPU new_cpu(uint8_t *memory, uint8_t memory_size) {
    CPU cpu = {0};
    
    memcpy(cpu.mem, memory, memory_size);
    
    return cpu;
};

void run_cpu(CPU *c) {
    while (c->IR != HLT) {
        fetchCycle(c);
        ALU(c);
    }
}

void set_flags(CPU *cpu, i32 result, i32 operand1, i32 operand2, i8 operation) {
    // 부호플래그
    cpu->flags.SF = (result < 0);

    // 제로 플래그
    cpu->flags.ZF = (result == 0);

    // 캐리플래그
    switch (operand1) {
        case ADD:
            if(result < operand1 || result < operand2) cpu->flags.CF = 1;
            break;
        case SUB:
            if(operand1 < operand2) cpu->flags.CF = 1;
            break;
        case MUL:
            if(result / operand1 != operand2) cpu->flags.CF = 1;
            break;
        case DIV:
            int quotient = operand1 / operand2;
            if(quotient != 0) cpu->flags.CF = 1;
    }

    // 오버플로우 플래그
    cpu->flags.OF = (result > INT_MAX || result < INT_MIN);

    // 패리티 플래그
    int count = 0;
    for (int i = 0; i < sizeof(result); i++) {
        if(result & (1 << i)) count++;
    }
    cpu->flags.PF = count % 2 == 0;

    //슈퍼바이저 플래그
    cpu->flags.AF = 0;
}

void clear_flags(CPU *cpu) {
    cpu->flags.ZF = 0;
    cpu->flags.CF = 0;
    cpu->flags.AF = 0;
    cpu->flags.OF = 0;
    cpu->flags.SF = 0;
    cpu->flags.PF = 0;
}

void ALU(CPU *cpu) {
    int pc_value = cpu->mem[cpu->PC];
    int pc_next_value = cpu->mem[cpu->PC+1];

    switch (cpu->IR) {
        case MOV: {
            cpu->reg[pc_value] = cpu->reg[pc_next_value];
            increment_pc(cpu, 2);
            break;
        };
        case ADD: {
            set_flags(cpu, cpu->reg[pc_value] + cpu->reg[pc_next_value], cpu->reg[pc_value], cpu->reg[pc_next_value], ADD);

            cpu->reg[pc_value] += cpu->reg[pc_next_value];
            increment_pc(cpu, 2);
            break;
        };
        case SUB: {
            set_flags(cpu, cpu->reg[pc_value] - cpu->reg[pc_next_value], cpu->reg[pc_value], cpu->reg[pc_next_value], SUB);

            cpu->reg[pc_value] -= cpu->reg[pc_next_value];
            increment_pc(cpu, 2);
            break;
        };
        case MUL: {
            set_flags(cpu, cpu->reg[pc_value] * cpu->reg[pc_next_value], cpu->reg[pc_value], cpu->reg[pc_next_value], MUL);

            cpu->reg[pc_value] *= cpu->reg[pc_next_value];
            increment_pc(cpu, 2);
            break;
        };
        case DIV: {
            set_flags(cpu, pc_value / pc_next_value, pc_value, pc_next_value, DIV);
            
            if(pc_next_value == 0) cpu->flags.ZF = 1;
            pc_value /= pc_next_value;

            increment_pc(cpu, 2);
            break;
        }
    }
}

void fetchCycle(CPU *cpu) {
    cpu->MAR = cpu->PC;
    cpu->MBR = cpu->mem[cpu->MAR];
    cpu->PC = cpu->PC + 1;
    cpu->IR = cpu->MBR;
}

void increment_pc(CPU *cpu, int value) {
    cpu->PC = cpu->PC + value;
}

void print_register(CPU *cpu) {
    for (int i = 0; i < sizeof(cpu->reg); i++) {
        printf("%u ", cpu->reg[i]);
    }
}

void print_memory(CPU *cpu) {
    for (int i = 0; i < sizeof(cpu->mem); i++) {
        printf("%u ", cpu->mem[i]);
    }
}

#endif