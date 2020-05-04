#include <iostream>
#include <iomanip>
#include "CPU.hh"

CPU::CPU(bool debug) : _halted(false), _debug(debug), _wait(0) {
	this->_registers.a = 0;
	this->_registers.x = 0;
	this->_registers.y = 0;
	this->_registers.sp = 0xff;
	this->_registers.p = 0;
	this->_registers.pc = 0;

	this->_op_table[0x00] = {nullptr, &CPU::impliedAddress, 1}; // 0x00
	this->_op_table[0x01] = {nullptr, &CPU::indirectXAddress, 1};
	this->_op_table[0x02] = {nullptr, nullptr, 1};
	this->_op_table[0x03] = {nullptr, nullptr, 1};
	this->_op_table[0x04] = {nullptr, nullptr, 1};
	this->_op_table[0x05] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x06] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x07] = {nullptr, nullptr, 1};
	this->_op_table[0x08] = {&CPU::PHP, &CPU::impliedAddress, 3};
	this->_op_table[0x09] = {nullptr, &CPU::immediateAddress, 1};
	this->_op_table[0x0a] = {nullptr, &CPU::accumulatorAddress, 1};
	this->_op_table[0x0b] = {nullptr, nullptr, 1};
	this->_op_table[0x0c] = {nullptr, nullptr, 1};
	this->_op_table[0x0d] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x0e] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x0f] = {nullptr, nullptr, 1};
	this->_op_table[0x10] = {nullptr, &CPU::relativeAddress, 1}; // 0x10
	this->_op_table[0x11] = {nullptr, &CPU::indirectYAddress, 1};
	this->_op_table[0x12] = {nullptr, nullptr, 1};
	this->_op_table[0x13] = {nullptr, nullptr, 1};
	this->_op_table[0x14] = {nullptr, nullptr, 1};
	this->_op_table[0x15] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x16] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x17] = {nullptr, nullptr, 1};
	this->_op_table[0x18] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0x19] = {nullptr, &CPU::absoluteYAddress, 1};
	this->_op_table[0x1a] = {nullptr, nullptr, 1};
	this->_op_table[0x1b] = {nullptr, nullptr, 1};
	this->_op_table[0x1c] = {nullptr, nullptr, 1};
	this->_op_table[0x1d] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0x1e] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0x1f] = {nullptr, nullptr, 1};
	this->_op_table[0x20] = {&CPU::JSR, &CPU::absoluteAddress, 6}; // 0x20
	this->_op_table[0x21] = {&CPU::AND, &CPU::indirectXAddress, 6};
	this->_op_table[0x22] = {nullptr, nullptr, 1};
	this->_op_table[0x23] = {nullptr, nullptr, 1};
	this->_op_table[0x24] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x25] = {&CPU::AND, &CPU::zeropageAddress, 3};
	this->_op_table[0x26] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x27] = {nullptr, nullptr, 1};
	this->_op_table[0x28] = {&CPU::PLP, &CPU::impliedAddress, 4};
	this->_op_table[0x29] = {&CPU::AND, &CPU::immediateAddress, 2};
	this->_op_table[0x2a] = {nullptr, &CPU::accumulatorAddress, 1};
	this->_op_table[0x2b] = {nullptr, nullptr, 1};
	this->_op_table[0x2c] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x2d] = {&CPU::AND, &CPU::absoluteAddress, 4};
	this->_op_table[0x2e] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x2f] = {nullptr, nullptr, 1};
	this->_op_table[0x30] = {nullptr, &CPU::relativeAddress, 1}; // 0x30
	this->_op_table[0x31] = {&CPU::AND, &CPU::indirectYAddress, 5};
	this->_op_table[0x32] = {nullptr, nullptr, 1};
	this->_op_table[0x33] = {nullptr, nullptr, 1};
	this->_op_table[0x34] = {nullptr, nullptr, 1};
	this->_op_table[0x35] = {&CPU::AND, &CPU::zeropageXAddress, 4};
	this->_op_table[0x36] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x37] = {nullptr, nullptr, 1};
	this->_op_table[0x38] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0x39] = {&CPU::AND, &CPU::absoluteYAddress, 4};
	this->_op_table[0x3a] = {nullptr, nullptr, 1};
	this->_op_table[0x3b] = {nullptr, nullptr, 1};
	this->_op_table[0x3c] = {nullptr, nullptr, 1};
	this->_op_table[0x3d] = {&CPU::AND, &CPU::absoluteXAddress, 4};
	this->_op_table[0x3e] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0x3f] = {nullptr, nullptr, 1};
	this->_op_table[0x40] = {nullptr, &CPU::impliedAddress, 1}; // 0x40
	this->_op_table[0x41] = {nullptr, &CPU::indirectXAddress, 1};
	this->_op_table[0x42] = {nullptr, nullptr, 1};
	this->_op_table[0x43] = {nullptr, nullptr, 1};
	this->_op_table[0x44] = {nullptr, nullptr, 1};
	this->_op_table[0x45] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x46] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x47] = {nullptr, nullptr, 1};
	this->_op_table[0x48] = {&CPU::PHA, &CPU::impliedAddress, 3};
	this->_op_table[0x49] = {nullptr, &CPU::immediateAddress, 1};
	this->_op_table[0x4a] = {nullptr, &CPU::accumulatorAddress, 1};
	this->_op_table[0x4b] = {nullptr, nullptr, 1};
	this->_op_table[0x4c] = {&CPU::JMP,   &CPU::absoluteAddress, 3};
	this->_op_table[0x4d] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x4e] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x4f] = {nullptr, nullptr, 1};
	this->_op_table[0x50] = {nullptr, &CPU::relativeAddress, 1}; // 0x50
	this->_op_table[0x51] = {nullptr, &CPU::indirectYAddress, 1};
	this->_op_table[0x52] = {nullptr, nullptr, 1};
	this->_op_table[0x53] = {nullptr, nullptr, 1};
	this->_op_table[0x54] = {nullptr, nullptr, 1};
	this->_op_table[0x55] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x56] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x57] = {nullptr, nullptr, 1};
	this->_op_table[0x58] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0x59] = {nullptr, &CPU::absoluteYAddress, 1};
	this->_op_table[0x5a] = {nullptr, nullptr, 1};
	this->_op_table[0x5b] = {nullptr, nullptr, 1};
	this->_op_table[0x5c] = {nullptr, nullptr, 1};
	this->_op_table[0x5d] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0x5e] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0x5f] = {nullptr, nullptr, 1};
	this->_op_table[0x60] = {&CPU::RTS, &CPU::impliedAddress, 6}; // 0x60
	this->_op_table[0x61] = {nullptr, &CPU::indirectXAddress, 1};
	this->_op_table[0x62] = {nullptr, nullptr, 1};
	this->_op_table[0x63] = {nullptr, nullptr, 1};
	this->_op_table[0x64] = {nullptr, nullptr, 1};
	this->_op_table[0x65] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x66] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x67] = {nullptr, nullptr, 1};
	this->_op_table[0x68] = {&CPU::PLA, &CPU::impliedAddress, 4};
	this->_op_table[0x69] = {nullptr, &CPU::immediateAddress, 1};
	this->_op_table[0x6a] = {nullptr, &CPU::accumulatorAddress, 1};
	this->_op_table[0x6b] = {nullptr, nullptr, 1};
	this->_op_table[0x6c] = {nullptr, &CPU::indirectAddress, 1};
	this->_op_table[0x6d] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x6e] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x6f] = {nullptr, nullptr, 1};
	this->_op_table[0x70] = {nullptr, &CPU::relativeAddress, 1}; // 0x70
	this->_op_table[0x71] = {nullptr, &CPU::indirectYAddress, 1};
	this->_op_table[0x72] = {nullptr, nullptr, 1};
	this->_op_table[0x73] = {nullptr, nullptr, 1};
	this->_op_table[0x74] = {nullptr, nullptr, 1};
	this->_op_table[0x75] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x76] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x77] = {nullptr, nullptr, 1};
	this->_op_table[0x78] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0x79] = {nullptr, &CPU::absoluteYAddress, 1};
	this->_op_table[0x7a] = {nullptr, nullptr, 1};
	this->_op_table[0x7b] = {nullptr, nullptr, 1};
	this->_op_table[0x7c] = {nullptr, nullptr, 1};
	this->_op_table[0x7d] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0x7e] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0x7f] = {nullptr, nullptr, 1};
	this->_op_table[0x80] = {nullptr, nullptr, 1}; // 0x80
	this->_op_table[0x81] = {&CPU::STA, &CPU::indirectXAddress, 6};
	this->_op_table[0x82] = {nullptr, nullptr, 1};
	this->_op_table[0x83] = {nullptr, nullptr, 1};
	this->_op_table[0x84] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x85] = {&CPU::STA, &CPU::zeropageAddress, 3};
	this->_op_table[0x86] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0x87] = {nullptr, nullptr, 1};
	this->_op_table[0x88] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0x89] = {nullptr, nullptr, 1};
	this->_op_table[0x8a] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0x8b] = {nullptr, nullptr, 1};
	this->_op_table[0x8c] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x8d] = {&CPU::STA, &CPU::absoluteAddress, 4};
	this->_op_table[0x8e] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0x8f] = {nullptr, nullptr, 1};
	this->_op_table[0x90] = {nullptr, &CPU::relativeAddress, 1}; // 0x90
	this->_op_table[0x91] = {&CPU::STA, &CPU::indirectYAddress, 6};
	this->_op_table[0x92] = {nullptr, nullptr, 1};
	this->_op_table[0x93] = {nullptr, nullptr, 1};
	this->_op_table[0x94] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x95] = {&CPU::STA, &CPU::zeropageXAddress, 4};
	this->_op_table[0x96] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0x97] = {nullptr, nullptr, 1};
	this->_op_table[0x98] = {&CPU::TYA, &CPU::impliedAddress, 2};
	this->_op_table[0x99] = {&CPU::STA, &CPU::absoluteYAddress, 5};
	this->_op_table[0x9a] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0x9b] = {nullptr, nullptr, 1};
	this->_op_table[0x9c] = {nullptr, nullptr, 1};
	this->_op_table[0x9d] = {&CPU::STA, &CPU::absoluteXAddress, 5};
	this->_op_table[0x9e] = {nullptr, nullptr, 1};
	this->_op_table[0x9f] = {nullptr, nullptr, 1};
	this->_op_table[0xa0] = {&CPU::LDY, &CPU::immediateAddress, 2}; // 0xa0
	this->_op_table[0xa1] = {&CPU::LDA, &CPU::indirectXAddress, 6};
	this->_op_table[0xa2] = {&CPU::LDX, &CPU::immediateAddress, 2};
	this->_op_table[0xa3] = {nullptr, nullptr, 1};
	this->_op_table[0xa4] = {&CPU::LDY, &CPU::zeropageAddress, 3};
	this->_op_table[0xa5] = {&CPU::LDA, &CPU::zeropageAddress, 3};
	this->_op_table[0xa6] = {&CPU::LDX, &CPU::zeropageAddress, 3};
	this->_op_table[0xa7] = {nullptr, nullptr, 1};
	this->_op_table[0xa8] = {&CPU::TAY, &CPU::impliedAddress, 2};
	this->_op_table[0xa9] = {&CPU::LDA, &CPU::immediateAddress, 2};
	this->_op_table[0xaa] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0xab] = {nullptr, nullptr, 1};
	this->_op_table[0xac] = {&CPU::LDY, &CPU::absoluteAddress, 4};
	this->_op_table[0xad] = {&CPU::LDA, &CPU::absoluteAddress, 4};
	this->_op_table[0xae] = {&CPU::LDX, &CPU::absoluteAddress, 4};
	this->_op_table[0xaf] = {nullptr, nullptr, 1};
	this->_op_table[0xb0] = {nullptr, &CPU::relativeAddress, 1}; // 0xb0
	this->_op_table[0xb1] = {&CPU::LDA, &CPU::indirectYAddress, 5};
	this->_op_table[0xb2] = {nullptr, nullptr, 1};
	this->_op_table[0xb3] = {nullptr, nullptr, 1};
	this->_op_table[0xb4] = {&CPU::LDY, &CPU::zeropageXAddress, 4};
	this->_op_table[0xb5] = {&CPU::LDA, &CPU::zeropageXAddress, 4};
	this->_op_table[0xb6] = {&CPU::LDX, &CPU::zeropageYAddress, 4};
	this->_op_table[0xb7] = {nullptr, nullptr, 1};
	this->_op_table[0xb8] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0xb9] = {&CPU::LDA, &CPU::absoluteYAddress, 4};
	this->_op_table[0xba] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0xbb] = {nullptr, nullptr, 1};
	this->_op_table[0xbc] = {&CPU::LDY, &CPU::absoluteXAddress, 4};
	this->_op_table[0xbd] = {&CPU::LDA, &CPU::absoluteXAddress, 4};
	this->_op_table[0xbe] = {&CPU::LDX, &CPU::absoluteYAddress, 4};
	this->_op_table[0xbf] = {nullptr, nullptr, 1};
	this->_op_table[0xc0] = {nullptr, &CPU::immediateAddress, 1}; // 0xc0
	this->_op_table[0xc1] = {&CPU::CMP, &CPU::indirectXAddress, 6};
	this->_op_table[0xc2] = {nullptr, nullptr, 1};
	this->_op_table[0xc3] = {nullptr, nullptr, 1};
	this->_op_table[0xc4] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0xc5] = {&CPU::CMP, &CPU::zeropageAddress, 3};
	this->_op_table[0xc6] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0xc7] = {nullptr, nullptr, 1};
	this->_op_table[0xc8] = {&CPU::INY, &CPU::impliedAddress, 2};
	this->_op_table[0xc9] = {&CPU::CMP, &CPU::immediateAddress, 2};
	this->_op_table[0xca] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0xcb] = {nullptr, nullptr, 1};
	this->_op_table[0xcc] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0xcd] = {&CPU::CMP, &CPU::absoluteAddress, 4};
	this->_op_table[0xce] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0xcf] = {nullptr, nullptr, 1};
	this->_op_table[0xd0] = {&CPU::BNE, &CPU::relativeAddress, 2}; // 0xd0
	this->_op_table[0xd1] = {&CPU::CMP, &CPU::indirectYAddress, 5};
	this->_op_table[0xd2] = {nullptr, nullptr, 1};
	this->_op_table[0xd3] = {nullptr, nullptr, 1};
	this->_op_table[0xd4] = {nullptr, nullptr, 1};
	this->_op_table[0xd5] = {&CPU::CMP, &CPU::zeropageXAddress, 4};
	this->_op_table[0xd6] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0xd7] = {nullptr, nullptr, 1};
	this->_op_table[0xd8] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0xd9] = {&CPU::CMP, &CPU::absoluteYAddress, 4};
	this->_op_table[0xda] = {nullptr, nullptr, 1};
	this->_op_table[0xdb] = {nullptr, nullptr, 1};
	this->_op_table[0xdc] = {nullptr, nullptr, 1};
	this->_op_table[0xdd] = {&CPU::CMP, &CPU::absoluteXAddress, 4};
	this->_op_table[0xde] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0xdf] = {nullptr, nullptr, 1};
	this->_op_table[0xe0] = {nullptr, &CPU::immediateAddress, 1}; // 0xe0
	this->_op_table[0xe1] = {nullptr, &CPU::indirectXAddress, 1};
	this->_op_table[0xe2] = {nullptr, nullptr, 1};
	this->_op_table[0xe3] = {nullptr, nullptr, 1};
	this->_op_table[0xe4] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0xe5] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0xe6] = {nullptr, &CPU::zeropageAddress, 1};
	this->_op_table[0xe7] = {nullptr, nullptr, 1};
	this->_op_table[0xe8] = {&CPU::INX, &CPU::impliedAddress, 2};
	this->_op_table[0xe9] = {nullptr, &CPU::immediateAddress, 1};
	this->_op_table[0xea] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0xeb] = {nullptr, nullptr, 1};
	this->_op_table[0xec] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0xed] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0xee] = {nullptr, &CPU::absoluteAddress, 1};
	this->_op_table[0xef] = {nullptr, nullptr, 1};
	this->_op_table[0xf0] = {&CPU::BEQ, &CPU::relativeAddress, 2}; // 0xf0
	this->_op_table[0xf1] = {nullptr, &CPU::indirectYAddress, 1};
	this->_op_table[0xf2] = {nullptr, nullptr, 1};
	this->_op_table[0xf3] = {nullptr, nullptr, 1};
	this->_op_table[0xf4] = {nullptr, nullptr, 1};
	this->_op_table[0xf5] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0xf6] = {nullptr, &CPU::zeropageXAddress, 1};
	this->_op_table[0xf7] = {nullptr, nullptr, 1};
	this->_op_table[0xf8] = {nullptr, &CPU::impliedAddress, 1};
	this->_op_table[0xf9] = {nullptr, &CPU::absoluteYAddress, 1};
	this->_op_table[0xfa] = {nullptr, nullptr, 1};
	this->_op_table[0xfb] = {nullptr, nullptr, 1};
	this->_op_table[0xfc] = {nullptr, nullptr, 1};
	this->_op_table[0xfd] = {nullptr, &CPU::absoluteXAddress, 1};
	this->_op_table[0xfe] = {nullptr, &CPU::absoluteXAddress, 1};
}

CPU::~CPU() {

}

bool CPU::isHalted() const {
	return this->_halted;
}

void CPU::addDevice(Device* device) {
	this->_devices.push_back(device);
}

registers_t CPU::getRegisters() const {
	return this->_registers;
}

void CPU::readResetVector() {
	this->_registers.pc = this->readFromDevice(0xfffc) + (this->readFromDevice(0xfffd) << 8);
}

void CPU::cycle() {
	if (!this->_wait)
		this->parseInstructiom(this->readFromDevice(this->_registers.pc));
	else
		this->_wait--;
}

void CPU::parseInstructiom(uint8_t instruction) {
	if (this->_debug)
		std::cout << std::setfill('0') << std::setw(4) << std::hex << this->_registers.pc << ":\t";
	this->_registers.pc++;
	CPU::op_t opcode = this->_op_table[instruction];
	if (opcode.opcode == nullptr) {
		std::cout << "Unknown instruction 0x" << std::setfill('0') << std::setw(2) << int(instruction) << std::endl;
		this->_halted = 1;
	}
	else {
		(this->*opcode.opcode)((this->*opcode.address)());
		this->_wait = opcode.cycles;
	}
	this->_wait--;
}

// Addresse modes
uint16_t CPU::accumulatorAddress() {
	return 0;
}

uint16_t CPU::impliedAddress() {
	return 0;
}

uint16_t CPU::immediateAddress() {
	return this->_registers.pc++;
}

uint16_t CPU::absoluteAddress() {
	return this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc++) << 8);
}

uint16_t CPU::absoluteXAddress() {
	return this->absoluteAddress() + this->_registers.x;
}

uint16_t CPU::absoluteYAddress() {
	return this->absoluteAddress() + this->_registers.y;
}

uint16_t CPU::zeropageAddress() {
	return this->readFromDevice(this->_registers.pc++) % 256;
}

uint16_t CPU::zeropageXAddress() {
	return (this->zeropageAddress() + this->_registers.x) % 256;
}

uint16_t CPU::zeropageYAddress() {
	return (this->zeropageAddress() + this->_registers.y) % 256;
}

uint16_t CPU::relativeAddress() {
	char relative = this->readFromDevice(this->_registers.pc++);
	return this->_registers.pc + relative;
}

uint16_t CPU::indirectAddress() {
	uint16_t address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc++) << 8);
	return this->readFromDevice(address) + (this->readFromDevice(address + 1) << 8);
}

uint16_t CPU::indirectXAddress() {
	uint16_t address = this->readFromDevice(this->_registers.pc++) + this->_registers.x;
	return this->readFromDevice(address) + (this->readFromDevice(address + 1) << 8);
}

uint16_t CPU::indirectYAddress() {
	uint16_t address = this->readFromDevice(this->_registers.pc++);
	return this->readFromDevice(address) + (this->readFromDevice(address + 1) << 8) + this->_registers.y;
}

// Opcodes
void CPU::AND(uint16_t address) {
	uint8_t byte = this->readFromDevice(address);
	this->_registers.a &= byte;
	this->updateFlagZero(this->_registers.a);
	this->updateFlagNegative(this->_registers.a);
	if (this->_debug)
		std::cout << "AND\t$" << uint16_t(byte) << std::endl;
}

void CPU::BEQ(uint16_t address) {
	if (this->_registers.p & ZERO)
		this->_registers.pc = address;
	if (this->_debug)
		std::cout << "BEQ\t$" << address << std::endl;
}

void CPU::BNE(uint16_t address) {
	if (!(this->_registers.p & ZERO))
		this->_registers.pc = address;
	if (this->_debug)
		std::cout << "BNE\t$" << address << std::endl;
}

void CPU::CMP(uint16_t address) {
	uint8_t byte = this->readFromDevice(address);
	this->updateFlagCarry(uint16_t(this->_registers.a) - byte);
	this->updateFlagZero(byte);
	this->updateFlagNegative(byte);
	if (this->_debug)
		std::cout << "CMP\t$" << address << "\t#$" << uint16_t(byte) << std::endl;
}

void CPU::INX(uint16_t address [[maybe_unused]]) {
	this->_registers.x++;
	this->updateFlagZero(this->_registers.x);
	this->updateFlagNegative(this->_registers.x);
	if (this->_debug)
		std::cout << "INX" << std::endl;
}

void CPU::INY(uint16_t address [[maybe_unused]]) {
	this->_registers.y++;
	this->updateFlagZero(this->_registers.y);
	this->updateFlagNegative(this->_registers.y);
	if (this->_debug)
		std::cout << "INY" << std::endl;
}

void CPU::JMP(uint16_t address) {
	this->_registers.pc = address;
	if (this->_debug)
		std::cout << "JMP\t$" << address << std::endl;
}

void CPU::JSR(uint16_t address) {
	this->writeToDevice(0xff + this->_registers.sp--, this->_registers.pc >> 8);
	this->writeToDevice(0xff + this->_registers.sp--, this->_registers.pc & 0xff);
	this->_registers.pc = address;
	if (this->_debug)
		std::cout << "JSR\t$" << address << std::endl;
}

void CPU::LDA(uint16_t address) {
	uint8_t byte = this->readFromDevice(address);
	this->_registers.a = byte;
	this->updateFlagZero(this->_registers.a);
	this->updateFlagNegative(this->_registers.a);
	if (this->_debug)
		std::cout << "LDA\t$" << address << "\t#$" << uint16_t(byte) << std::endl;
}

void CPU::LDX(uint16_t address) {
	uint8_t byte = this->readFromDevice(address);
	this->_registers.x = byte;
	this->updateFlagZero(this->_registers.x);
	this->updateFlagNegative(this->_registers.x);
	if (this->_debug)
		std::cout << "LDX\t$" << address << "\t#$" << uint16_t(byte) << std::endl;
}

void CPU::LDY(uint16_t address) {
	uint8_t byte = this->readFromDevice(address);
	this->_registers.y = byte;
	this->updateFlagZero(this->_registers.y);
	this->updateFlagNegative(this->_registers.y);
	if (this->_debug)
		std::cout << "LDY\t$" << address << "\t#$" << uint16_t(byte) << std::endl;
}

void CPU::PHA(uint16_t address [[maybe_unused]]) {
	this->writeToDevice(0xff + this->_registers.sp--, this->_registers.a);
	if (this->_debug)
		std::cout << "PHA" << std::endl;
}

void CPU::PHP(uint16_t address [[maybe_unused]]) {
	this->writeToDevice(0xff + this->_registers.sp--, this->_registers.p);
	if (this->_debug)
		std::cout << "PHP" << std::endl;
}

void CPU::PLA(uint16_t address [[maybe_unused]]) {
	this->_registers.a = this->readFromDevice(0xff + ++this->_registers.sp);
	this->updateFlagZero(this->_registers.a);
	this->updateFlagNegative(this->_registers.a);
	if (this->_debug)
		std::cout << "PLA" << std::endl;
}

void CPU::PLP(uint16_t address [[maybe_unused]]) {
	this->_registers.p = this->readFromDevice(0xff + ++this->_registers.sp);
	if (this->_debug)
		std::cout << "PLP" << std::endl;
}

void CPU::RTS(uint16_t address [[maybe_unused]]) {
	this->_registers.pc = this->readFromDevice(0xff + ++this->_registers.sp);
	this->_registers.pc += this->readFromDevice(0xff + ++this->_registers.sp) << 8;
	if (this->_debug)
		std::cout << "RST" << std::endl;
}

void CPU::STA(uint16_t address) {
	this->writeToDevice(address, this->_registers.a);
	if (this->_debug)
		std::cout << "STA\t$" << address << std::endl;
}

void CPU::TAY(uint16_t address [[maybe_unused]]) {
	this->_registers.y = this->_registers.a;
	this->updateFlagZero(this->_registers.y);
	this->updateFlagNegative(this->_registers.y);
	if (this->_debug)
		std::cout << "TAY" << std::endl;
}

void CPU::TYA(uint16_t address [[maybe_unused]]) {
	this->_registers.a = this->_registers.y;
	this->updateFlagZero(this->_registers.a);
	this->updateFlagNegative(this->_registers.a);
	if (this->_debug)
		std::cout << "TYA" << std::endl;
}

// Flags
void CPU::updateFlagCarry(uint16_t value) {
	if (value > 0xff)
		this->_registers.p |= ZERO;
	else
		this->_registers.p &= ~ZERO;
}

void CPU::updateFlagZero(uint8_t value) {
	if (!value)
		this->_registers.p |= ZERO;
	else
		this->_registers.p &= ~ZERO;
}

void CPU::updateFlagNegative(uint8_t value) {
	if (value & 0x80)
		this->_registers.p |= NEGATIVE;
	else
		this->_registers.p &= ~NEGATIVE;
}

uint8_t CPU::readFromDevice(uint16_t address) {
	Device* device = this->getDevice(address);
	if (device == nullptr) {
		std::cout << "WARNING: no device at this address, except garbage data (0x" << std::setfill('0') << std::setw(4) << std::hex << address << ")" << std::endl;
		return rand() % 256;
	}
	return device->readByte(address);
}

void CPU::writeToDevice(uint16_t address, uint8_t byte) {
	Device* device = this->getDevice(address);
	if (device == nullptr) {
		std::cout << "WARNING: no device at this address, write to nothing (0x" << std::setfill('0') << std::setw(4) << std::hex << address << ")" << std::endl;
		return;
	}
	device->writeByte(address, byte);
}

Device* CPU::getDevice(uint16_t address) {
	for (Device* device: this->_devices) {
		if (address >= device->getOffset() && address < device->getOffset() + device->getSize())
			return device;
	}
	return nullptr;
}

std::vector<Device*> CPU::getDevices() const {
	return this->_devices;
}

std::ostream& operator<<(std::ostream& os, const CPU& cpu) {
	os << "Device mapping:" << std::endl;
	for (Device* device: cpu.getDevices()) {
		os << "    " << device->getName() << ":\t0x" << std::setfill('0') << std::setw(4) << std::hex << device->getOffset() << " - 0x" << std::setfill('0') << std::setw(4) << std::hex << device->getOffset() + device->getSize() - 1 << std::endl;
	}
	os << "Registers:" << std::endl;
	os << "    a:\t0x" << std::setfill('0') << std::setw(2) << std::hex << int(cpu.getRegisters().a) << std::endl;
	os << "    x:\t0x" << std::setfill('0') << std::setw(2) << std::hex << int(cpu.getRegisters().x) << std::endl;
	os << "    y:\t0x" << std::setfill('0') << std::setw(2) << std::hex << int(cpu.getRegisters().y) << std::endl;
	os << "    sp:\t0x" << std::setfill('0') << std::setw(2) << std::hex << int(cpu.getRegisters().sp) << std::endl;
	os << "    p:\t0x" << std::setfill('0') << std::setw(2) << std::hex << int(cpu.getRegisters().p) << std::endl;
	os << "    pc:\t0x" << std::setfill('0') << std::setw(4) << std::hex << int(cpu.getRegisters().pc) << std::endl;
	return os;
}
