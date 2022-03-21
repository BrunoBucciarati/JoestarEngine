#pragma once
#include "../Base/ObjectDefines.h"
#include "../Base/SubSystem.h"
#include "../Container/Variant.h"
#include "../Container/HashMap.h"
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
		HashMap<U32, Variant> configMap;
	};


	template<typename T>
	bool GlobalConfig::UpdateConfig(GlobalConfigDef key, T& value)
	{
		configMap[(U32)key] = Variant(value);
		return true;
	}

	template<typename T>
	T GlobalConfig::GetConfig(GlobalConfigDef key)
	{
		U32 k = key;
		auto it = configMap.Find(k);
		if (configMap.Contains(k))
		{
			return T(configMap[k]);
		}
		return NULL;
	}
}