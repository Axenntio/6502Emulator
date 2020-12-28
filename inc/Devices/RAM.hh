#include <vector>
#include "inc/Device.hh"

#ifndef __RAM_HH__
#define __RAM_HH__

class RAM : public Device {
public:
	RAM(uint16_t busPosition, uint16_t size);
	~RAM();
};


#endif