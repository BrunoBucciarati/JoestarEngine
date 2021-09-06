#pragma once
#include "../Base/SubSystem.h"
#include <map>
#include "../Math/Matrix4x4.h"
#include "../Base/StringHash.h"
#define REGISTER_UNIFROM_DATA(CLASS) \
	k##CLASS,
namespace Joestar {
	enum UNIFORM_DATAS {
		kUnifromBufferStart = 0,
		REGISTER_UNIFROM_DATA(UniformBufferObject)

		kPushConstStart,
		REGISTER_UNIFROM_DATA(PushConsts)
		REGISTER_UNIFROM_DATA(ComputeSHConsts)

		kInvalidUniformData
	};
	struct PushConsts {
		Matrix4x4f model;
	};

	struct UniformBufferObject {
		//Matrix4x4f model;
		Matrix4x4f view;
		Matrix4x4f proj;
		bool operator== (UniformBufferObject& ub) {
			return view == ub.view && proj == ub.proj;//&& model == ub.model;
		}
	};

	struct ComputeSHConsts {
		U32 coefs[9];
	};

	//class UniformBufferPool : public SubSystem {
	//	REGISTER_SUBSYSTEM(UniformBufferPool)
	//public:
	//	explicit UniformBufferPool(EngineContext* ctx);
	//	
	//private:
	//	std::map<U32, std::vector<void*>> elementPool;
	//};
}