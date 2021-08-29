#include "GameObject.h"

namespace Joestar {
	GameObject::GameObject(EngineContext* ctx) : Super(ctx) {
		GetComponent<Transform>();
	}
}