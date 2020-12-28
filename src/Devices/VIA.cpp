#include <iostream>
#include <iomanip>
#include "inc/Devices/VIA.hh"

VIA::VIA(uint16_t busPosition, const std::string& accessMode) : Device("VIA", busPosition, accessMode) {
	this->_memory = std::vector<uint8_t>(16, 0);
	this->_memory[2] = 0x00;
	this->_memory[3] = 0x00;
}

VIA::~VIA() {
}

uint8_t VIA::readByte(uint16_t index) {
	if (this->_busPosition > index || index - this->_busPosition > uint16_t(this->_memory.size())) {
		std::cerr << "WARNING: access to an unflashed byte, will get garbage at this point" << std::endl;
		return rand() % 256;
	}
	return this->_memory[index - this->_busPosition];
}

void VIA::writeByte(uint16_t index, uint8_t value) {
	if (this->_busPosition > index || this->_busPosition - index > uint16_t(this->_memory.size())) {
		std::cerr << "ERROR: Seem you are out of device" << std::endl;
		return;
	}
	this->_memory[index - this->_busPosition] = value;
}