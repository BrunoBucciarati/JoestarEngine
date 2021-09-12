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
		Vector4f cameraPos; // for alignment
		Vector4f sunDirection;
	};

	struct UniformBufferObjectPS {
		Vector4f cameraPos; // for alignment
		Vector4f sunDirection;
		Vector4f sunColor;
	};

	const int MAX_LIGHTS = 20;
	struct LightBlocks {
		Vector4f lightPos[MAX_LIGHTS];
		Vector4f lightColors[MAX_LIGHTS];
		U32 lightCount;
		float padding[3];
		Vector3f shCoef[9];
	};

	struct ComputeSHConsts {
		U32 sizeAndLevel[3];
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