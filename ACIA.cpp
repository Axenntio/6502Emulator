#include <iostream>
#include <iomanip>
#include "ACIA.hh"

ACIA::ACIA(uint16_t bus_position) : Device("ACIA", bus_position) {
	this->_memory = std::vector<uint8_t>(4, 0);
	this->_memory[1] = 0x10;
}

ACIA::~ACIA() {
}


uint8_t ACIA::getByte(uint16_t index) const {
	if (this->_bus_position > index || index - this->_bus_position > uint16_t(this->_memory.size())) {
		std::cout << "WARNING: access to an unflashed byte, will get garbage at this point" << std::endl;
		return rand() % 256;
	}
	return this->_memory[index - this->_bus_position];
}

void ACIA::writeByte(uint16_t index, uint8_t value) {
	if (this->_bus_position > index || this->_bus_position - index > uint16_t(this->_memory.size())) {
		std::cout << "ERROR: Seem you are out of ram" << std::endl;
		return;
	}
	this->_memory[index - this->_bus_position] = value;
	switch (this->_bus_position - index) {
		case 0:
			std::cout << value << std::flush;
			break;
		default:
			break;
	}
}