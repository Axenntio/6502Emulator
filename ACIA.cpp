#include <iostream>
#include <iomanip>
#include "ACIA.hh"

ACIA::ACIA(uint16_t bus_position) : Device("ACIA", bus_position) {
	this->_memory = std::vector<uint8_t>(4, 0);
	this->_memory[1] |= 0x10;
}

ACIA::~ACIA() {
}


uint8_t ACIA::readByte(uint16_t index) {
	if (this->_bus_position > index || index - this->_bus_position > uint16_t(this->_memory.size())) {
		std::cout << "WARNING: access to an unflashed byte, will get garbage at this point" << std::endl;
		return rand() % 256;
	}
	switch (index - this->_bus_position) {
		case 0:
			if (this->_message.size()) {
				this->_memory[index - this->_bus_position] = char(this->_message.substr(0, 1)[0]);
				this->_message = this->_message.substr(1, this->_message.size() - 1);
			}
			if (!this->_message.size())
				this->_memory[1] &= ~0x08;
			break;
		default:
			break;
	}
	return this->_memory[index - this->_bus_position];
}

void ACIA::writeByte(uint16_t index, uint8_t value) {
	if (this->_bus_position > index || this->_bus_position - index > uint16_t(this->_memory.size())) {
		std::cout << "ERROR: Seem you are out of device" << std::endl;
		return;
	}
	this->_memory[index - this->_bus_position] = value;
	switch (index - this->_bus_position) {
		case 0:
			std::cout << value << std::flush;
			break;
		default:
			break;
	}
}

void ACIA::sendChars(std::string message) {
	this->_message = message;
	this->_memory[1] |= 0x08;
}