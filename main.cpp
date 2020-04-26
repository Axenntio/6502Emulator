#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "CPU.hh"
#include "EEPROM.hh"
#include "RAM.hh"
#include "ACIA.hh"

bool running = true;
std::string message;
bool has_message = false;

void enable_raw_mode() {
	termios term;

	tcgetattr(0, &term);
	term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode() {
	termios term;

	tcgetattr(0, &term);
	term.c_lflag |= ICANON | ECHO;
	tcsetattr(0, TCSANOW, &term);
}

bool kbhit() {
	int byteswaiting;

	ioctl(0, FIONREAD, &byteswaiting);
	return byteswaiting > 0;
}

char getch() {
	char buf = 0;
	struct termios old;

	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror ("read()");
	return buf;
}

void signal_callback_handler(int signum) {
	if (signum == SIGINT) {
		running = false;
	}
	else if (signum == SIGTSTP) {
		if (!has_message) {
			// Will probably need to create some command to dump
			has_message = true;
			disable_raw_mode();
			std::getline(std::cin, message);
			enable_raw_mode();
			message += "\n";
		}
	}
}


int main(int argc, char **argv) {
	signal(SIGINT, signal_callback_handler);
	signal(SIGTSTP, signal_callback_handler);
	enable_raw_mode();
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
		if (kbhit()) {
			acia.sendChar(getch());
			//std::cout << "There is in hex: " << std::hex << int(getch()) << std::endl;
		}
		if (has_message) {
			acia.sendChars(message);
			has_message = false;
		}
		cpu.cycle();
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}	
	// std::cout << cpu << std::endl;
	// std::cout << ram << std::endl;
	// std::cout << acia << std::endl;
	// std::cout << eeprom << std::endl;
	disable_raw_mode();
	return 0;
}