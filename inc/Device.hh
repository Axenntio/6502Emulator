#include <string>
#include <vector>

#ifndef __DEVICE_HH__
#define __DEVICE_HH__

class Device {
public:
	Device(std::string name, uint16_t bus_position);
	virtual ~Device();

	std::string getName() const;
	uint16_t getOffset() const;
	uint16_t getSize() const;
	virtual uint8_t readByte(uint16_t index);
	virtual void writeByte(uint16_t index, uint8_t value);

protected:
	std::string _name;
	uint16_t _bus_position;
	std::vector<uint8_t> _memory;
};

std::ostream& operator<<(std::ostream& os, Device& Device);

#endif