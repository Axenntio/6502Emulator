#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include "inc/Devices/EEPROM.hh"

EEPROM::EEPROM(uint16_t bus_position, std::string file) : Device("EEPROM", bus_position) {
	std::ifstream infile(file, std::ios_base::binary);

	if (!infile.is_open()) {
		std::cerr << "Failed loading EEPROM file" << std::endl;
		return;
	}
    
	this->_memory = std::vector<uint8_t>(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());
}

EEPROM::~EEPROM() {
}

void EEPROM::writeByte(uint16_t index, [[maybe_unused]] uint8_t value) {
	if (this->_bus_position > index || this->_bus_position - index > uint16_t(this->_memory.size())) {
		std::cerr << "ERROR: Seem you are out of device" << std::endl;
		return;
	}
	std::cerr << "ERROR: you are trying to write on the EEPROM" << std::endl;
}