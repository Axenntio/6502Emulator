#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include "Device.hh"

Device::Device(std::string name, uint16_t bus_position) : _name(name), _bus_position(bus_position) {
}

Device::~Device() {
}

std::string Device::getName() const {
	return this->_name;
}

uint16_t Device::getOffset() const {
	return this->_bus_position;
}

uint16_t Device::getSize() const {
	return this->_memory.size();
}

uint8_t Device::readByte(uint16_t index) {
	if (this->_bus_position > index || index - this->_bus_position > uint16_t(this->_memory.size())) {
		std::cout << "WARNING: access to an unflashed byte, will get garbage at this point" << std::endl;
		return rand() % 256;
	}
	return this->_memory[index - this->_bus_position];
}

void Device::writeByte(uint16_t index, uint8_t value) {
	if (this->_bus_position > index || this->_bus_position - index > uint16_t(this->_memory.size())) {
		std::cout << "ERROR: Seem you are out of ram" << std::endl;
		return;
	}
	this->_memory[index - this->_bus_position] = value;
}

std::ostream& operator<<(std::ostream& os, Device& device) {
	std::cout << device.getName() << std::endl;
	std::cout << "0x" << std::setfill('0') << std::setw(4) << std::hex << device.getOffset() << " - 0x" << device.getOffset() + device.getSize();
	for (uint16_t i = 0; i < device.getSize(); i++) {
		if (!(i % 16))
			os << std::endl << std::setfill('0') << std::setw(4) << std::hex << i << ":";
		if (!(i % 2))
			os << " ";
		os << std::setfill('0') << std::setw(2) << std::hex << int(device.readByte(i + device.getOffset()));
	}
	return os << std::endl;;
}
