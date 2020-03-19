#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include "CPU.hh"
#include "EEPROM.hh"
#include "RAM.hh"
#include "ACIA.hh"

bool running = true;
std::string message;
bool has_message = false;

void signal_callback_handler(int signum) {
	if (signum == SIGINT) {
		running = false;
	}
	else if (signum == SIGTSTP) {
		if (!has_message) {
			has_message = true;
			std::getline(std::cin, message);
			//message = "m 8000";
			message += "\n";
		}
	}
}


int main(int argc, char **argv) {
	signal(SIGINT, signal_callback_handler);
	signal(SIGTSTP, signal_callback_handler);
	std::string file = "firmware";
	if (argc == 2)
		file = std::string(argv[1]);
	EEPROM eeprom(0x8000, file);
	RAM ram(0x0000, 0x4000);
	ACIA acia(0x6000);
	CPU cpu(false);
	cpu.addDevice(&ram);
	cpu.addDevice(&acia);
	cpu.addDevice(&eeprom);
	cpu.readResetVector();
	std::cout << cpu << std::endl;
	while (!cpu.isHalted() && running) {
		if (has_message) {
			acia.sendChars(message);
			has_message = false;
		}
		cpu.cycle();
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}	
	// std::cout << cpu << std::endl;
	// std::cout << acia << std::endl;
	// std::cout << ram << std::endl;
	// std::cout << eeprom << std::endl;
	return 0;
}