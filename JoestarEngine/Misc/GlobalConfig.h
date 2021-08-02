#pragma once
#include <map>
#include <stdint.h>
class GlobalConfig {
public:
	template<typename T>
	bool UpdateConfig(const char* key, T& value) {
		if (configMap.find(key) != configMap.end())
		{
			configMap[key] = (void*)&value;
		}
		else
			configMap.insert(std::pair<const char*, void*>(key, &value));
		return true;
	}
	template<typename T>
	T GetConfig(const char* key) {
		if (configMap.find(key) != configMap.end())
		{
			return *(reinterpret_cast<T*>(configMap[key]));
		}
		return NULL;
	}
private:
	std::map<const char*, void*> configMap;
};


static GlobalConfig* gConfig;
inline GlobalConfig* GetGlobalConfig() {
	if (!gConfig) {
		gConfig = new GlobalConfig;
	}
	return gConfig;
}