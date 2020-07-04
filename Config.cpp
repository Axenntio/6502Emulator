#include <iostream>
#include <fstream>
#include <map>
#include <any>
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
		std::map<std::string, std::any> params;
		params.insert(std::pair<std::string, std::any>("file", std::string("")));
		params.insert(std::pair<std::string, std::any>("start", uint16_t(0)));
		params.insert(std::pair<std::string, std::any>("size", uint16_t(0)));

		for (const auto& kv : params) {
			if (kv.second.type() == typeid(uint16_t)) {
				if (device_json[kv.first] != Json::nullValue) {
					if (device_json[kv.first].isNumeric())
						params[kv.first] = (uint16_t)device_json[kv.first].asUInt();
					else
						params[kv.first] = (uint16_t)strtol(device_json[kv.first].asCString(), NULL, 16);
				}
			}
			else if (kv.second.type() == typeid(std::string)) {
				if (device_json[kv.first] != Json::nullValue) {
					params[kv.first] = device_json[kv.first].asString();
				}
			}
		}

		if (device_name == "EEPROM") {
			device = new EEPROM(std::any_cast<uint16_t>(params["start"]), std::any_cast<std::string>(params["file"]));
		}
		if (device_name == "RAM") {
			device = new RAM(std::any_cast<uint16_t>(params["start"]), std::any_cast<uint16_t>(params["size"]));
		}
		if (device_name == "ACIA") {
			device = new ACIA(std::any_cast<uint16_t>(params["start"]));
		}
		cpu.addDevice(device);
	}
	cpu.readResetVector();

	return cpu;
}