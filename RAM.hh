#include <vector>
#include "Device.hh"

#ifndef __RAM_HH__
#define __RAM_HH__

class RAM : public Device{
public:
	RAM(uint16_t bus_position, uint16_t size);
	~RAM();
};


#endif