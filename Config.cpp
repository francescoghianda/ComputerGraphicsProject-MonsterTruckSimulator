#include "Config.h"


Config::Config(std::string configFilePath) {
	this->readConfigs(configFilePath);
}

void Config::readConfigs(std::string configFilePath) {
	std::ifstream configFile;
	configFile.open(configFilePath);

	std::string line;
	while (std::getline(configFile, line)) {

		std::istringstream is_line(line);
		std::string key;
		if (!line.empty() && std::getline(is_line, key, '='))
		{
			std::string value;
			if (std::getline(is_line, value))
				this->configs[key] = value;
			
		}

	}
}

std::string Config::get(std::string key) {

	if (this->configs.count(key) <= 0) return "";

	return this->configs[key];
}

int Config::getInt(std::string key) {
	std::string value = this->get(key);

	return std::stoi(value);
}

float Config::getFloat(std::string key) {
	std::string value = this->get(key);
	return std::stof(value);
}

bool Config::getBool(std::string key) {
	std::string value = this->get(key);
	bool b;
	std::istringstream(value) >> std::boolalpha >> b;
	return b;
}

glm::vec3 Config::getVec3(std::string key) {
	std::string value = this->get(key);

	if (value == "#") return glm::vec3(0);

	std::istringstream is(value);
	glm::vec3 vec;

	for (int i = 0; i < 3; i++) {
		std::string val;
		if (!std::getline(is, val, ','))
			val = "0";
		vec[i] = std::stof(val);
	}
	return vec;
}
