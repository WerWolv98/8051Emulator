// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "instr_sizes.h"

uint8_t mem_flash[0x10080];
uint8_t mem_ram[0x100];
uint8_t mem_xram[0x10000];

uint8_t regBank = 0;


#define FLASH(addr) (mem_flash[addr])
#define RAM(addr)   (mem_ram[addr])
#define XRAM(addr)  (mem_xram[addr])

#define P0   RAM(0x80)  // GPIO Port 0
#define SP   RAM(0x81)  // Stack Pointer
#define DPL  RAM(0x82)  // Data Pointer Low
#define DPH  RAM(0x83)  // Data Pointer High
#define PCON RAM(0x87)  // Power Control
#define TCON RAM(0x88)  // Timer Control
#define TMOD RAM(0x89)  // Timer Mode
#define TL0  RAM(0x8A)  // Timer 0 Low
#define TL1  RAM(0x8B)  // Timer 1 Low
#define TH0  RAM(0x8C)  // Timer 0 High
#define TH1  RAM(0x8D)  // Timer 1 High
#define P1   RAM(0x90)  // GPIO Port 1
#define SCON RAM(0x98)  // Serial Control
#define SBUF RAM(0x99)  // Serial Buffer
#define P2   RAM(0xA0)  // GPIO Port 2
#define IE   RAM(0xA8)  // Interrupt Enable
#define P3   RAM(0xB0)  // GPIO Port 3
#define IP   RAM(0xB8)  // Interrupt Priority
#define PSW  RAM(0xD0)  // Program Status Word
#define ACC  RAM(0xE0)  // Accumulator
#define B    RAM(0xF0)  // B Register

#define R0   RAM(0x00 + 0x08 * regBank)
#define R1   RAM(0x01 + 0x08 * regBank)
#define R2   RAM(0x02 + 0x08 * regBank)
#define R3   RAM(0x03 + 0x08 * regBank)
#define R4   RAM(0x04 + 0x08 * regBank)
#define R5   RAM(0x05 + 0x08 * regBank)
#define R6   RAM(0x06 + 0x08 * regBank)
#define R7   RAM(0x07 + 0x08 * regBank)

uint16_t PC; // Program Counter

void startup() {
	PC = 0x0000;
	SP = 0x07;
}

uint8_t get_bit(uint8_t bit) {
	if (bit <= 0x7F)  // Bit RAM
		return (RAM(0x20 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0x87) // P0
		return (RAM(0x80 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0x8F) // TCON
		return (RAM(0x88 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0x9F) // P1
		return (RAM(0x90 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0xAF) // SCON + P2
		return (RAM(0xA0 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0xBF) // P3 + IP
		return (RAM(0xB0 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0xCF) // Unassigned
		return 0;
	else if (bit <= 0xD7) // PSW
		return (RAM(0xD0 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0xDF) // Unassigned
		return 0;
	else if (bit <= 0xE7) // ACC
		return (RAM(0xE0 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else if (bit <= 0xEF) // Unassigned
		return 0;
	else if (bit <= 0xF7) // B
		return (RAM(0xF0 + (bit >> 3)) & (1 << (bit % 8))) > 0;
	else return 0;
}

void set_bit(uint8_t bit) {
	if (bit <= 0x7F)  // Bit RAM
		RAM(0x20 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0x87) // P0
		RAM(0x80 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0x8F) // TCON
		RAM(0x88 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0x9F) // P1
		RAM(0x90 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0xAF) // SCON + P2
		RAM(0xA0 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0xBF) // P3 + IP
		RAM(0xB0 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0xCF) // Unassigned
		;
	else if (bit <= 0xD7) // PSW
		RAM(0xD0 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0xDF) // Unassigned
		;
	else if (bit <= 0xE7) // ACC
		RAM(0xE0 + (bit >> 3)) |= (1 << (bit % 8));
	else if (bit <= 0xEF) // Unassigned
		;
	else if (bit <= 0xF7) // B
		RAM(0xF0 + (bit >> 3)) |= (1 << (bit % 8));
}

void clr_bit(uint8_t bit) {
	if (bit <= 0x7F)  // Bit RAM
		RAM(0x20 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0x87) // P0
		RAM(0x80 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0x8F) // TCON
		RAM(0x88 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0x9F) // P1
		RAM(0x90 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0xAF) // SCON + P2
		RAM(0xA0 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0xBF) // P3 + IP
		RAM(0xB0 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0xCF) // Unassigned
		;
	else if (bit <= 0xD7) // PSW
		RAM(0xD0 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0xDF) // Unassigned
		;
	else if (bit <= 0xE7) // ACC
		RAM(0xE0 + (bit >> 3)) &= ~(1 << (bit % 8));
	else if (bit <= 0xEF) // Unassigned
		;
	else if (bit <= 0xF7) // B
		RAM(0xF0 + (bit >> 3)) &= ~(1 << (bit % 8));
}

void execInstr(uint8_t instr) {
	uint16_t newPC;
	uint8_t bit;
	bool jump_instr;

	switch (instr) {
	case 0x00: // NOP
		break;
	case 0x01: // AJMP
		newPC = PC;
		newPC &= ~0x7FF;
		newPC |= FLASH(PC + 1);
		PC = newPC;
		jump_instr = true;
		break;
	case 0x02: // LJMP
		newPC = PC;
		newPC = FLASH(PC + 1);
		newPC |= FLASH(PC + 2) << 8;
		PC = newPC;
		jump_instr = true;
		break;
	case 0x03: // RR
		bit = (ACC & 0x01) << 7;
		ACC >>= 1;
		ACC |= bit;
		break;
	case 0x04: // INC ACC
		ACC++;
		break;
	case 0x05: // INC IRAM ADDR
		RAM(FLASH(PC + 1))++;
		break;
	case 0x06: // INC @R0
		RAM(R0)++;
		break;
	case 0x07: // INC @R1
		RAM(R1)++;
		break;
	case 0x08: // INC R0
		R0++;
		break;
	case 0x09: // INC R1
		R1++;
		break;
	case 0x0A: // INC R2
		R2++;
		break;
	case 0x0B: // INC R3
		R3++;
		break;
	case 0x0C: // INC R4
		R4++;
		break;
	case 0x0D: // INC R5
		R5++;
		break;
	case 0x0E: // INC R6
		R6++;
		break;
	case 0x0F: // INC R7
		R7++;
		break;
	case 0x10: // JBC
		if (get_bit(FLASH(PC + 1))) {
			clr_bit(FLASH(PC + 1));
			PC = FLASH(PC + 2);
			jump_instr = true;
		}
		break;
	}

	if (!jump_instr)
		PC += instr_size[instr];
}

int main()
{

	startup();

	std::cin.get();
}
