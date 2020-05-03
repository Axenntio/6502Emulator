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
	uint16_t address = 0;
	uint8_t byte = 0;

	if (this->_debug)
		std::cout << std::setfill('0') << std::setw(4) << std::hex << this->_registers.pc << ":\t";
	this->_registers.pc++;
	switch (instruction) {
	case 0x00: // BRK, need reimplementation (use to debug)
		this->_halted = true;
		break;
	case 0x05: // ORA zeropage (3)
		address = this->readFromDevice(this->_registers.pc++);
		byte = this->readFromDevice(address);
		this->_registers.a |= byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 3;
		if (this->_debug)
			std::cout << "ORA\t$" << int(address) << std::endl;
		break;
	case 0x0a: // ASL accumulator (2)
		byte = (this->_registers.a << 1);
		if (this->_registers.a & 0x80)
			this->_registers.p |= CARRY;
		else
			this->_registers.p &= ~CARRY;
		this->_registers.a = byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "ASL\tA" << std::endl;
		break;
	case 0x18: // CLC implied (2)
		this->_registers.p &= ~CARRY;
		this->_wait = 2;
		if (this->_debug)
			std::cout << "CLC" << std::endl;
		break;
	case 0x20: // JSR absolute (6)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc) << 8);
		this->writeToDevice(0xff + this->_registers.sp--, this->_registers.pc >> 8);
		this->writeToDevice(0xff + this->_registers.sp--, this->_registers.pc & 0xff);
		this->_registers.pc = address;
		this->_wait = 6;
		if (this->_debug)
			std::cout << "JSR\t$" << int(address) << std::endl;
		break;
	case 0x29: // AND immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		this->_registers.a &= byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "AND\t#$" << int(byte) << std::endl;
		break;
	case 0x38: // SEC implied (2)
		this->_registers.p |= CARRY;
		this->_wait = 2;
		if (this->_debug)
			std::cout << "SEC" << std::endl;
		break;
	case 0x48: // PHA implied (3)
		this->writeToDevice(0xff + this->_registers.sp--, this->_registers.a);
		this->_wait = 3;
		if (this->_debug)
			std::cout << "PHA" << std::endl;
		break;
	case 0x4a: // LSR accumulator (2)
		if (this->_registers.a & 0x01)
			this->_registers.p |= CARRY;
		else
			this->_registers.p &= ~CARRY;
		this->_registers.a >>= 1;
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "LRS\tA" << std::endl;
		break;
	case 0x4c: // JMP absolute (3)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc) << 8);
		this->_registers.pc = address;
		this->_wait = 3;
		if (this->_debug)
			std::cout << "JMP\t$" << int(address) << std::endl;
		break;
	case 0x60: // RTS implied (6)
		this->_registers.pc = this->readFromDevice(0xff + ++this->_registers.sp);
		this->_registers.pc += this->readFromDevice(0xff + ++this->_registers.sp) << 8;
		this->_registers.pc++;
		this->_wait = 6;
		if (this->_debug)
			std::cout << "RTS" << std::endl;
		break;
	case 0x65: // ADC zeropage (3)
		address = this->readFromDevice(this->_registers.pc++);
		byte = this->readFromDevice(address);
		if (int(this->_registers.a) + byte + (this->_registers.p & CARRY) < 0) {
			this->_registers.p |= CARRY;
			this->_registers.p |= OVERFLOW;
		}
		else
			this->_registers.p &= ~OVERFLOW;
		this->_registers.a += byte + (this->_registers.p & CARRY);
		this->_registers.p &= ~CARRY;
		this->updateFlag(this->_registers.a);
		this->_wait = 3;
		if (this->_debug)
			std::cout << "ADC\t$" << int(byte) << std::endl;
		break;
	case 0x68: // PLA implied  (4)
		this->_registers.a = this->readFromDevice(0xff + ++this->_registers.sp);
		this->updateFlag(this->_registers.a);
		this->_wait = 4;
		if (this->_debug)
			std::cout << "PLA" << std::endl;
		break;
	case 0x69: // ADC immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		if (int(this->_registers.a) + byte + (this->_registers.p & CARRY) > 255) {
			this->_registers.p |= CARRY;
			this->_registers.p |= OVERFLOW;
		}
		else
			this->_registers.p &= ~OVERFLOW;
		this->_registers.a += byte + (this->_registers.p & CARRY);
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "ADC\t#$" << int(byte) << std::endl;
		break;
	case 0x6c: // JMP indirect (5)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc) << 8);
		address = this->readFromDevice(address) + (this->readFromDevice(address + 1) << 8);
		this->_registers.pc = address;
		this->_wait = 5;
		if (this->_debug)
			std::cout << "JMP\t($" << int(address) << ")" << std::endl;
		break;
	case 0x85: // STA zeropage (3)
		address = this->readFromDevice(this->_registers.pc++);
		this->writeToDevice(address, this->_registers.a);
		this->_wait = 3;
		if (this->_debug)
			std::cout << "STA\t$" << int(address) << std::endl;
		break;
	case 0x88: // DEY implied (2)
		this->_registers.y--;
		this->updateFlag(this->_registers.y);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "DEY" << std::endl;
		break;
	case 0x8a: // TXA implied (2)
		this->_registers.a = this->_registers.x;
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "TXA" << std::endl;
		break;
	case 0x8d: // STA absolute (4)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc++) << 8);
		this->writeToDevice(address, this->_registers.a);
		this->_wait = 4;
		if (this->_debug)
			std::cout << "STA\t$" << int(address) << std::endl;
		break;
	case 0x90: // BCC relative (2)
		byte = this->readFromDevice(this->_registers.pc++);
		if (!(this->_registers.p & CARRY))
			this->_registers.pc += char(byte);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "BCC\t$" << int(byte) << std::endl;
		break;
	case 0x91: // STA (indirect), Y (6)
		byte = this->readFromDevice(this->_registers.pc++);
		address = this->readFromDevice(byte) + (this->readFromDevice(byte + 1) << 8) + this->_registers.y;
		this->writeToDevice(address, this->_registers.a);
		this->_wait = 6;
		if (this->_debug)
			std::cout << "STA\t($" << int(address) << "), Y"  << std::endl;
		break;
	case 0x98: // TYA implied (2)
		this->_registers.a = this->_registers.y;
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "TYA" << std::endl;
		break;
	case 0x99: // STA absolute, Y (5)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc++) << 8);
		this->writeToDevice(address + this->_registers.y, this->_registers.a);
		this->_wait = 5;
		if (this->_debug)
			std::cout << "STA\t$" << int(address) << ", Y"  << std::endl;
		break;
	case 0x9a: // TXS implied (2)
		this->_registers.sp = this->_registers.x;
		this->_wait = 2;
		if (this->_debug)
			std::cout << "TXS" << std::endl;
		break;
	case 0x9d: // STA absolute, X (5)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc++) << 8);
		this->writeToDevice(address + this->_registers.x, this->_registers.a);
		this->_wait = 5;
		if (this->_debug)
			std::cout << "STA\t$" << int(address) << ", X"  << std::endl;
		break;
	case 0xa0: // LDY immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		this->_registers.y = byte;
		this->updateFlag(this->_registers.y);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "LDY\t#$" << int(byte) << std::endl;
		break;
	case 0xa2: // LDX immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		this->_registers.x = byte;
		this->updateFlag(this->_registers.x);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "LDX\t#$" << int(byte) << std::endl;
		break;
	case 0xa5: // LDA zeropage (3)
		address = this->readFromDevice(this->_registers.pc++);
		byte = this->readFromDevice(address);
		this->_registers.a = byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 3;
		if (this->_debug)
			std::cout << "LDA\t$" << int(address) << std::endl;
		break;
	case 0xa8: // TAY implied (2)
		this->_registers.y = this->_registers.a;
		this->updateFlag(this->_registers.y);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "TAY" << std::endl;
		break;
	case 0xa9: // LDA immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		this->_registers.a = byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "LDA\t#$" << int(byte) << std::endl;
		break;
	case 0xad: // LDA absolute (4)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc++) << 8);
		byte = this->readFromDevice(address);
		this->_registers.a = byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 4;
		if (this->_debug)
			std::cout << "LDA\t$" << int(byte) << std::endl;
		break;
	case 0xb0: // BCS relative (2**)
		byte = this->readFromDevice(this->_registers.pc++);
		if (this->_registers.p & CARRY)
			this->_registers.pc += char(byte);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "BCS\t$" << int(byte) << std::endl;
		break;
	case 0xb1: // LDA (indirect), Y (5*)
		byte = this->readFromDevice(this->_registers.pc++);
		address = this->readFromDevice(byte) + (this->readFromDevice(byte + 1) << 8) + this->_registers.y;
		if (this->_debug)
			std::cout << "LDA\t($" << int(byte) << "), Y"  << std::endl;
		byte = this->readFromDevice(address);
		this->_registers.a = byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 5;
		break;
	case 0xbd: // LDA absolute, X (4*)
		address = this->readFromDevice(this->_registers.pc++) + (this->readFromDevice(this->_registers.pc++) << 8);
		byte = this->readFromDevice(address + this->_registers.x);
		this->_registers.a = byte;
		this->updateFlag(this->_registers.a);
		this->_wait = 4;
		if (this->_debug)
			std::cout << "LDA\t$" << int(address) << ", X"  << std::endl;
		break;
	case 0xc0: // CPY immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		if (this->_registers.y >= byte)
			this->_registers.p |= CARRY;
		else
			this->_registers.p &= ~CARRY;
		this->updateFlag(this->_registers.y - byte);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "CPY\t#$" << int(byte) << std::endl;
		break;
	case 0xc4: // CPY zeropage (3)
		address = this->readFromDevice(this->_registers.pc++);
		byte = this->readFromDevice(address);
		if (this->_registers.y >= byte)
			this->_registers.p |= CARRY;
		else
			this->_registers.p &= ~CARRY;
		this->updateFlag(this->_registers.y - byte);
		this->_wait = 3;
		if (this->_debug)
			std::cout << "CPY\t$" << int(address) << std::endl;
		break;
	case 0xc8: // INY implied (2)
		this->_registers.y++;
		this->updateFlag(this->_registers.y);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "INY" << std::endl;
		break;
	case 0xc9: // CMP immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		if (this->_registers.a >= byte)
			this->_registers.p |= CARRY;
		else
			this->_registers.p &= ~CARRY;
		this->updateFlag(this->_registers.a - byte);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "CMP\t#$" << int(byte) << std::endl;
		break;
	case 0xca: // DEX implied (2)
		this->_registers.x--;
		this->updateFlag(this->_registers.x);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "DEX" << std::endl;
		break;
	case 0xd0: // BNE relative (2**)
		byte = this->readFromDevice(this->_registers.pc++);
		if (!(this->_registers.p & ZERO))
			this->_registers.pc += char(byte);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "BNE\t$" << int(byte) << std::endl;
		break;
	case 0xd8: // CLD implied (2)
		this->_registers.p &= ~DECIMAL;
		this->_wait = 2;
		if (this->_debug)
			std::cout << "CLD" << std::endl;
		break;
	case 0xe5: // SBC zeropage (3)
		address = this->readFromDevice(this->_registers.pc++);
		byte = this->readFromDevice(address);
		if (int(this->_registers.a) - (byte + (this->_registers.p & CARRY)) < 0)
			this->_registers.p |= OVERFLOW;
		else
			this->_registers.p &= ~OVERFLOW;
		this->_registers.a -= (byte + (1 - (this->_registers.p & CARRY)));
		if (this->_registers.a & 0x80)
			this->_registers.p |= CARRY;
		else
			this->_registers.p &= ~CARRY;
		this->updateFlag(this->_registers.a);
		this->_wait = 3;
		if (this->_debug)
			std::cout << "SBC\t$" << int(byte) << std::endl;
		break;
	case 0xe8: // INX implied (2)
		this->_registers.x++;
		this->updateFlag(this->_registers.x);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "INX" << std::endl;
		break;
	case 0xe9: // SBC immediate (2)
		byte = this->readFromDevice(this->_registers.pc++);
		if (int(this->_registers.a) - (byte + (this->_registers.p & CARRY)) < 0)
			this->_registers.p |= OVERFLOW;
		else
			this->_registers.p &= ~OVERFLOW;
		this->_registers.a -= (byte + (1 - (this->_registers.p & CARRY)));
		this->updateFlag(this->_registers.a);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "SBC\t#$" << int(byte) << std::endl;
		break;
	case 0xea: // NOP, need reimplementation (use to debug)
		std::cout << *this << std::endl;
		this->_wait = 2;
		break;
	case 0xf0: // BEQ relative (2**)
		byte = this->readFromDevice(this->_registers.pc++);
		if (this->_registers.p & ZERO)
			this->_registers.pc += char(byte);
		this->_wait = 2;
		if (this->_debug)
			std::cout << "BEQ\t$" << int(byte) << std::endl;
		break;
	default:
		std::cout << "Unknown instruction 0x" << std::setfill('0') << std::setw(2) << int(instruction) << std::endl;
		this->_wait = 1;
	}
	this->_wait--;
}

void CPU::updateFlag(uint8_t value) {
	if (char(value) == 0)
		this->_registers.p |= ZERO;
	else
		this->_registers.p &= ~ZERO;
	
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
