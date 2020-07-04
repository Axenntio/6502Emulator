#include <string>
#include <vector>
#include "inc/Device.hh"

#ifndef __EEPROM_HH__
#define __EEPROM_HH__

class EEPROM : public Device {
public:
	EEPROM(uint16_t bus_position, std::string file);
	~EEPROM();

	void writeByte(uint16_t index, uint8_t value);
};

#endif