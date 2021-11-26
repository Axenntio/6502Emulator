#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "inc/Config.hh"
#include "inc/CPU.hh"

bool running = true;
CPU* cpuPtr;

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
		std::cerr << "getch:tcsetattr()" << std::endl;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		std::cerr << "getch:tcsetattr ICANON" << std::endl;
	if (read(0, &buf, 1) < 0)
		std::cerr << "getch:read()" << std::endl;
	return buf;
}

void signal_callback_handler(int signum) {
	if (signum == SIGINT) {
		running = false;
	}
	else if (signum == SIGTSTP) {
		cpuPtr->toggleDebug();
	}
	else if (signum == SIGQUIT){
		cpuPtr->reset();
	}
}

int main(int argc, char **argv) {
	signal(SIGINT, signal_callback_handler);
	signal(SIGTSTP, signal_callback_handler);
	signal(SIGQUIT, signal_callback_handler);
	enable_raw_mode();
	std::string file = "config.json";
	if (argc == 2)
		file = std::string(argv[1]);
	Config config(file);
	CPU cpu = config.create_cpu();
	cpuPtr = &cpu;
	std::cout << cpu << std::endl;
	while (!cpu.isHalted() && running) {
		if (kbhit()) {
			if (!cpu.sendCharToAcia(getch())) {
				std::cerr << "You don't have any ACIA to send value" << std::endl;
			}
			//std::cout << "There is in hex: " << std::hex << int(getch()) << std::endl;
		}
		cpu.cycle();
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
	disable_raw_mode();
	return 0;
}