#pragma once
#include <map>
#include "../Base/ObjectDefines.h"
#include <stdint.h>
//#include "../Base/EngineContext.h"
#include "../Base/SubSystem.h"
namespace Joestar {
	class GlobalConfig : public SubSystem {
	public:
		REGISTER_SUBSYSTEM(GlobalConfig)

		explicit GlobalConfig(EngineContext* context);
		template<typename T>
		bool UpdateConfig(const char* key, T& value);
		template<typename T>
		T GetConfig(const char* key);
	private:
		std::map<const char*, void*> configMap;
	};


	template<typename T>
	bool GlobalConfig::UpdateConfig(const char* key, T& value) {
		if (configMap.find(key) != configMap.end())
		{
			configMap[key] = (void*)&value;
		}
		else
			configMap.insert(std::pair<const char*, void*>(key, &value));
		return true;
	}

	template<typename T>
	T GlobalConfig::GetConfig(const char* key) {
		if (configMap.find(key) != configMap.end())
		{
			return *(reinterpret_cast<T*>(configMap[key]));
		}
		return NULL;
	}
}