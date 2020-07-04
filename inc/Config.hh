#include <json/json.h>
#include "inc/CPU.hh"

#ifndef __CONFIG_HH__
#define __CONFIG_HH__

class Config {
public:
	Config(std::string config_file);

	CPU create_cpu() const;
private:
	bool _debug;
	std::string _path;
	std::vector<Json::Value> _devices;
};

#endif