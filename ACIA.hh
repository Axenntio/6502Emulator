#include <string>
#include <vector>
#include "Device.hh"

#ifndef __ACIA_HH__
#define __ACIA_HH__

class ACIA : public Device{
public:
	ACIA(uint16_t bus_position);
	~ACIA();

	uint8_t getByte(uint16_t index) const;
	void writeByte(uint16_t index, uint8_t value);
};

#endif