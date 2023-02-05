#pragma once
#include<iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Config
{
private:
	std::map<std::string, std::string> configs;
	void readConfigs(std::string configFilePath);

public:
	Config(std::string configFilePath);

	std::string get(std::string key);
	int getInt(std::string key);
	float getFloat(std::string key);
	bool getBool(std::string key);
	glm::vec3 getVec3(std::string key);
	
};

