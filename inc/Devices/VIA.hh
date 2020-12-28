#include <string>
#include <vector>
#include "inc/Device.hh"

#ifndef __VIA_HH__
#define __VIA_HH__

class VIA : public Device {
public:
	VIA(uint16_t busPosition, const std::string& accessMode = "RW");
	~VIA();

	uint8_t readByte(uint16_t index);
	void writeByte(uint16_t index, uint8_t value);
};

#endif