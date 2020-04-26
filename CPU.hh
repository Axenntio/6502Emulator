#include <vector>
#include "Device.hh"

#ifndef __CPU_HH__
#define __CPU_HH__

#define NEGATIVE	128
#define OVERFLOW	64
#define IGNORED		32
#define BREAK		16
#define DECIMAL		8
#define INTERRUPT	4
#define ZERO		2
#define CARRY		1 

typedef struct {
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t sp;
	uint8_t p;
	uint16_t pc;
} registers_t;

class CPU {
public:
	CPU(bool debug);
	~CPU();

	void addDevice(Device* device);
	registers_t getRegisters() const;
	std::vector<Device*> getDevices() const;
	void readResetVector();
	void cycle();

	uint8_t readFromDevice(uint16_t address);
	void writeToDevice(uint16_t address, uint8_t byte);
	void updateFlag(uint8_t value);


	bool isHalted() const;

private:
	Device* getDevice(uint16_t address);
	void parseInstructiom(uint8_t instruction);

	registers_t _registers;
	bool _halted;
	bool _debug;
	uint8_t _wait;

	std::vector<Device*> _devices;
};

std::ostream& operator<<(std::ostream& os, const CPU& cpu);

#endif