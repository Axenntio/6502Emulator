#include <iostream>
#include <iomanip>
#include "inc/Devices/RAM.hh"

RAM::RAM(uint16_t bus_position, uint16_t size) : Device("RAM", bus_position) {
	this->_memory = std::vector<uint8_t>(size, 0);
}

RAM::~RAM() {
}