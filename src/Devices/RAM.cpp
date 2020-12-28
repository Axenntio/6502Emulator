#include <iostream>
#include <iomanip>
#include "inc/Devices/RAM.hh"

RAM::RAM(uint16_t busPosition, uint16_t size) : Device("RAM", busPosition, "RW") {
	this->_memory = std::vector<uint8_t>(size, 0);
}

RAM::~RAM() {
}