#include <iostream>
#include <fstream>
#include <map>
#include <any>
#include <filesystem>
#include "inc/Config.hh"
#include "inc/CPU.hh"
#include "inc/Device.hh"
#include "inc/Devices/EEPROM.hh"
#include "inc/Devices/RAM.hh"
#include "inc/Devices/ACIA.hh"
#include "inc/Devices/VIA.hh"

Config::Config(const std::string& filename) {
	this->_debug = false;
	this->_path = std::filesystem::path(filename).parent_path();
	if (!this->_path.empty())
		this->_path += "/";
	std::ifstream configFile(filename, std::ifstream::binary);
	if (!configFile.is_open()) {
		throw std::runtime_error("Failed loading config file (" + filename + ")");
	}
	Json::Value config;
	configFile >> config;
	this->_debug = config["CPU"]["debug"].asBool();
	for (auto device : config["CPU"]["devices"])
		this->_devices.push_back(device);
}

CPU Config::create_cpu() const {
	CPU cpu(this->_debug);
	for (const Json::Value& jsonDevice : this->_devices) {
		Device* device = nullptr;
		std::map<std::string, std::any> params;

		for (const std::string& key : jsonDevice.getMemberNames()) {
			if (jsonDevice[key].isNumeric()) {
				params.insert(std::pair<std::string, std::any>(key, static_cast<uint16_t>(jsonDevice[key].asUInt())));
			}
			else if (jsonDevice[key].asString().substr(0, 2) == "0x") {
				params.insert(std::pair<std::string, std::any>(key, static_cast<uint16_t>(strtol(jsonDevice[key].asCString(), NULL, 16))));
			}
			else {
				params.insert(std::pair<std::string, std::any>(key, jsonDevice[key].asString()));
			}
		}

		if (std::any_cast<std::string>(params["type"]) == "EEPROM") {
			device = new EEPROM(std::any_cast<uint16_t>(params["start"]), this->_path + std::any_cast<std::string>(params["file"]));
		}
		else if (std::any_cast<std::string>(params["type"]) == "RAM") {
			device = new RAM(std::any_cast<uint16_t>(params["start"]), std::any_cast<uint16_t>(params["size"]));
		}
		else if (std::any_cast<std::string>(params["type"]) == "ACIA") {
			device = new ACIA(std::any_cast<uint16_t>(params["start"]));
		}
		else if (std::any_cast<std::string>(params["type"]) == "VIA") {
			if (params.find("access_mode") != params.end()) {
				device = new VIA(std::any_cast<uint16_t>(params["start"]), std::any_cast<std::string>(params["access_mode"]));
			}
			else {
				device = new VIA(std::any_cast<uint16_t>(params["start"]));
			}
		}
		else {
			throw std::runtime_error("Device named '" + std::any_cast<std::string>(params["type"]) + "' doesn't exist");
		}
		cpu.addDevice(device);
	}
	cpu.readResetVector();

	return cpu;
}