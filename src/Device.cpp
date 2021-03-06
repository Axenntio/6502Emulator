#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "inc/Device.hh"

Device::Device(const std::string& name, uint16_t busPosition, const std::string& accessMode) : _name(name), _busPosition(busPosition), _accessMode(accessMode) {
}

Device::~Device() {
}

const std::string& Device::getName() const {
	return this->_name;
}

uint16_t Device::getOffset() const {
	return this->_busPosition;
}

uint16_t Device::getSize() const {
	return this->_memory.size();
}

const std::string& Device::getAccessMode() const {
	return this->_accessMode;
}

uint8_t Device::readByte(uint16_t index) {
	if (this->_busPosition > index || index - this->_busPosition > uint16_t(this->_memory.size())) {
		std::cerr << "WARNING: access to an unflashed byte, will get garbage at this point" << std::endl;
		return rand() % 256;
	}
	return this->_memory[index - this->_busPosition];
}

void Device::writeByte(uint16_t index, uint8_t value) {
	if (this->_busPosition > index || this->_busPosition - index > uint16_t(this->_memory.size())) {
		std::cerr << "ERROR: Seem you are out of device" << std::endl;
		return;
	}
	this->_memory[index - this->_busPosition] = value;
}

std::ostream& operator<<(std::ostream& os, Device& device) {
	std::cout << device.getName() << std::endl;
	std::cout << "0x" << std::setfill('0') << std::setw(4) << std::hex << device.getOffset() << " - 0x" << device.getOffset() + device.getSize() - 1;
	for (uint16_t i = 0; i < device.getSize(); i += 16) {
		os << std::endl << std::setfill('0') << std::setw(4) << std::hex << i + device.getOffset() << ":";
		for (uint16_t j = i; j < i + 16; j++) {
			if (j < device.getSize()) {
				if (!(j % 2))
					os << " ";
				os << std::setfill('0') << std::setw(2) << std::hex << int(device.readByte(j + device.getOffset()));
			}
		}
		os << "\t";
		for (uint16_t j = i; j < i + 16; j++) {
			if (j < device.getSize()) {
				if (std::isprint(device.readByte(j + device.getOffset())))
					os << device.readByte(j + device.getOffset());
				else
					os << ".";
			}
		}
	}
	return os << std::endl;;
}
