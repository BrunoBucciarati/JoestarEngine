#pragma once
#include "../Core/Minimal.h"
#include "../Component/Component.h"
#include "../Graphics/PipelineState.h"
#include "../Graphics/CommandBuffer.h"
#include "../Graphics/MaterialInstance.h"
#include "../Math/AABB.h"
namespace Joestar {
	class Graphics;
	class GraphicsPipelineState;
	class Renderer : public Component {
		REGISTER_COMPONENT_ROOT(Renderer);
	public:
		void Init();
		virtual void Render(CommandBuffer*) = 0;
		virtual SharedPtr<GraphicsPipelineState> GetPipelineState(CommandBuffer* cb);
		void SetMaterial(MaterialInstance* mat)
		{
			mMaterial = mat;
		}

		void SetMaterial(const String& vs, const String& ps);
		void SetMaterial(const String& vsps);

		MaterialInstance* GetMaterial()
		{
			return mMaterial;
		}

		void SetCastShadow(bool flag)
		{
			bCastShadow = flag;
		}

		bool GetCastShadow() const
		{
			return bCastShadow;
		}
	protected:
		WeakPtr<Graphics> mGraphics;
		Vector<SharedPtr<GraphicsPipelineState>> mPSOs;
		SharedPtr<MaterialInstance> mMaterial;
		bool bCastShadow{ false };
	};
}