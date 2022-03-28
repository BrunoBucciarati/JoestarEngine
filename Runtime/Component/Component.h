#pragma once
#include "../Core/Object.h"

namespace Joestar {
	class GameObject;
	class Component : public Object {
		REGISTER_OBJECT(Component, Object)
	public:
		explicit Component(EngineContext* ctx, GameObject* go) : Super(ctx), mGameObject(go) {

		}
		virtual void Init() {}

	protected:
		GameObject* mGameObject;
	};
}