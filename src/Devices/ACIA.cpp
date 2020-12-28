#include <iostream>
#include <iomanip>
#include "inc/Devices/ACIA.hh"

ACIA::ACIA(uint16_t busPosition) : Device("ACIA", busPosition, "RW") {
	this->_memory = std::vector<uint8_t>(4, 0);
	this->_memory[1] |= 0x10;
}

ACIA::~ACIA() {
}

uint8_t ACIA::readByte(uint16_t index) {
	if (this->_busPosition > index || index - this->_busPosition > uint16_t(this->_memory.size())) {
		std::cerr << "WARNING: access to an unflashed byte, will get garbage at this point" << std::endl;
		return rand() % 256;
	}
	switch (index - this->_busPosition) {
		case 0:
			if (this->_message.size()) {
				this->_memory[index - this->_busPosition] = char(this->_message.substr(0, 1)[0]);
				this->_message = this->_message.substr(1, this->_message.size() - 1);
			}
			if (!this->_message.size())
				this->_memory[1] &= ~0x08;
			break;
		default:
			break;
	}
	return this->_memory[index - this->_busPosition];
}

void ACIA::writeByte(uint16_t index, uint8_t value) {
	if (this->_busPosition > index || this->_busPosition - index > uint16_t(this->_memory.size())) {
		std::cerr << "ERROR: Seem you are out of device" << std::endl;
		return;
	}
	this->_memory[index - this->_busPosition] = value;
	switch (index - this->_busPosition) {
		case 0:
			std::cout << value << std::flush;
			break;
		default:
			break;
	}
}

void ACIA::sendChars(const std::string& message) {
	this->_message = message;
	this->_memory[1] |= 0x08;
}

void ACIA::sendChar(char c) {
	this->_message += c;
	this->_memory[1] |= 0x08;
}