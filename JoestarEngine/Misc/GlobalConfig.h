#pragma once
#include <map>
#include "../Base/ObjectDefines.h"
#include <stdint.h>
//#include "../Base/EngineContext.h"
#include "../Base/SubSystem.h"
namespace Joestar {
	enum GlobalConfigDef {
		CONFIG_GFX_API = 0,
		CONFIG_WINDOW_WIDTH,
		CONFIG_WINDOW_HEIGHT
	};

	class GlobalConfig : public SubSystem {
	public:
		REGISTER_SUBSYSTEM(GlobalConfig)

		explicit GlobalConfig(EngineContext* context);
		template<typename T>
		bool UpdateConfig(GlobalConfigDef key, T& value);
		template<typename T>
		T GetConfig(GlobalConfigDef);
	private:
		std::map<GlobalConfigDef, void*> configMap;
	};


	template<typename T>
	bool GlobalConfig::UpdateConfig(GlobalConfigDef key, T& value) {
		if (configMap.find(key) != configMap.end())
		{
			configMap[key] = (void*)&value;
		}
		else
			configMap.insert(std::pair<GlobalConfigDef, void*>(key, &value));
		return true;
	}

	template<typename T>
	T GlobalConfig::GetConfig(GlobalConfigDef key) {
		if (configMap.find(key) != configMap.end())
		{
			return *(reinterpret_cast<T*>(configMap[key]));
		}
		return NULL;
	}
}