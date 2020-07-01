#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "Config.hh"
#include "CPU.hh"

bool running = true;
CPU* cpu_ptr;

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
		cpu_ptr->toggleDebug();
	}
}

int main(int argc, char **argv) {
	signal(SIGINT, signal_callback_handler);
	signal(SIGTSTP, signal_callback_handler);
	enable_raw_mode();
	std::string file = "config.json";
	if (argc == 2)
		file = std::string(argv[1]);
	Config config(file);
	CPU cpu = config.create_cpu();
	cpu_ptr = &cpu;
	while (!cpu.isHalted() && running) {
		if (kbhit()) {
			//acia.sendChar(getch());
			//std::cout << "There is in hex: " << std::hex << int(getch()) << std::endl;
		}
		cpu.cycle();
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
	std::cout << cpu << std::endl;
	disable_raw_mode();
	return 0;
}