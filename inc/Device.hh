#include <string>
#include <vector>

#ifndef __DEVICE_HH__
#define __DEVICE_HH__

class Device {
public:
	Device(const std::string& name, uint16_t busPosition, const std::string& accessMode);
	virtual ~Device();

	const std::string& getName() const;
	uint16_t getOffset() const;
	uint16_t getSize() const;
	const std::string& getAccessMode() const;
	virtual uint8_t readByte(uint16_t index);
	virtual void writeByte(uint16_t index, uint8_t value);

protected:
	std::string _name;
	uint16_t _busPosition;
	std::vector<uint8_t> _memory;
	std::string _accessMode;
};

std::ostream& operator<<(std::ostream& os, Device& Device);

#endif