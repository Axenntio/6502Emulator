#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include "CPU.hh"
#include "EEPROM.hh"
#include "RAM.hh"
#include "ACIA.hh"

bool running = true;

void signal_callback_handler(int signum) {
	running = false;
}

int main(int argc, char **argv) {
	signal(SIGINT, signal_callback_handler);
	std::string file = "firmware";
	if (argc == 2)
		file = std::string(argv[1]);
	EEPROM eeprom(0x8000, file);
	RAM ram(0x0000, 0x100);
	ACIA acia(0x07f00);
	CPU cpu(false);
	cpu.addDevice(&acia);
	cpu.addDevice(&ram);
	cpu.addDevice(&eeprom);
	cpu.readResetVector();
	std::cout << cpu << std::endl;
	while (!cpu.isHalted() && running) {
		//std::cout << cpu << std::endl;
		cpu.cycle();
		//std::cout << ram << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}	
	std::cout << cpu << std::endl;
	std::cout << acia << std::endl;
	std::cout << ram << std::endl;
	return 0;
}