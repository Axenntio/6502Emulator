#include <iostream>
#include <fstream>
#include "Config.hh"
#include "CPU.hh"
#include "Device.hh"
#include "EEPROM.hh"
#include "RAM.hh"
#include "ACIA.hh"

Config::Config(std::string filename) {
	this->_debug = false;
	std::ifstream config_file(filename, std::ifstream::binary);
	if (!config_file.is_open()) {
		std::cerr << "Failed loading config file" << std::endl;
		return;
	}
	Json::Value config;
	config_file >> config;
	this->_debug = config["CPU"]["debug"].asBool();
	for (auto device : config["CPU"]["devices"])
		this->_devices.push_back(device);
}

CPU Config::create_cpu() const {
	CPU cpu(this->_debug);
	for (auto device_json : this->_devices) {
		std::string device_name = device_json["type"].asString();
		Device* device = nullptr;
		if (device_name == "EEPROM") {
			device = new EEPROM(device_json["start"].asUInt(), device_json["file"].asString());
		}
		if (device_name == "RAM") {
			device = new RAM(device_json["start"].asUInt(), device_json["end"].asUInt());
		}
		if (device_name == "ACIA") {
			device = new ACIA(device_json["start"].asUInt());
		}
		cpu.addDevice(device);
	}
	cpu.readResetVector();

	return cpu;
}