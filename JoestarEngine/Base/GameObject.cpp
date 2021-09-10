#include "GameObject.h"

namespace Joestar {
	static U32 gGOIdx = 0;
	GameObject::GameObject(EngineContext* ctx) : Super(ctx) {
		GetComponent<Transform>();
		//give a default name
		SetName("Game Object " + gGOIdx);
	}

	GameObject::~GameObject() {
		for (auto& comp : components) {
			comp->Destroy();
		}
	}
}