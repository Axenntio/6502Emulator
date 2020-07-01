#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include "EEPROM.hh"

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