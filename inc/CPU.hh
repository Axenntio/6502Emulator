#include <vector>
#include "inc/Device.hh"

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
	bool sendCharToAcia(char c);
	void cycle();

	uint8_t readFromDevice(uint16_t address);
	void writeToDevice(uint16_t address, uint8_t byte);

	void setDebug(bool active);
	void toggleDebug();
	bool isDebug() const;

	bool isHalted() const;

private:
	Device* getDevice(uint16_t address);
	void parseInstructiom(uint8_t instruction);

	// Addresse modes
	uint16_t accumulatorAddress();
	uint16_t impliedAddress();
	uint16_t immediateAddress();
	uint16_t absoluteAddress();
	uint16_t absoluteXAddress();
	uint16_t absoluteYAddress();
	uint16_t zeropageAddress();
	uint16_t zeropageXAddress();
	uint16_t zeropageYAddress();
	uint16_t relativeAddress();
	uint16_t indirectAddress();
	uint16_t indirectXAddress();
	uint16_t indirectYAddress();

	// Opcodes
	void ADC(uint16_t address);
	void AND(uint16_t address);
	void ASL(uint16_t address);
	void ASL_ACC(uint16_t address);
	void BCC(uint16_t address);
	void BCS(uint16_t address);
	void BEQ(uint16_t address);
	void BNE(uint16_t address);
	void BRK(uint16_t address);
	void CLC(uint16_t address);
	void CLD(uint16_t address);
	void CMP(uint16_t address);
	void CPX(uint16_t address);
	void CPY(uint16_t address);
	void DEX(uint16_t address);
	void DEY(uint16_t address);
	void INX(uint16_t address);
	void INY(uint16_t address);
	void JMP(uint16_t address);
	void JSR(uint16_t address);
	void LDA(uint16_t address);
	void LDX(uint16_t address);
	void LDY(uint16_t address);
	void LSR(uint16_t address);
	void LSR_ACC(uint16_t address);
	void NOP(uint16_t address);
	void ORA(uint16_t address);
	void PHA(uint16_t address);
	void PHP(uint16_t address);
	void PLA(uint16_t address);
	void PLP(uint16_t address);
	void RTS(uint16_t address);
	void SBC(uint16_t address);
	void SEC(uint16_t address);
	void STA(uint16_t address);
	void TAY(uint16_t address);
	void TXS(uint16_t address);
	void TYA(uint16_t address);

	// Flags
	void updateFlagCarry(uint16_t value);
	void updateFlagZero(uint8_t value);
	void updateFlagNegative(uint8_t value);
	void setFlagCarry(bool active);
	void setFlagOverflow(bool active);

	typedef struct {
		void (CPU::*opcode)(uint16_t address);
		uint16_t (CPU::*address)();
		uint8_t cycles;
	} op_t;

	registers_t _registers;
	bool _halted;
	bool _debug;
	uint8_t _wait;
	op_t _op_table[0xff];

	std::vector<Device*> _devices;
};

std::ostream& operator<<(std::ostream& os, const CPU& cpu);

#endif