#pragma once
#include "GameObject.hpp"
#include "SDL_mixer.h"


class MedkitComponent : public Component {
public:
	explicit MedkitComponent(GameObject* gameObject);
	void update(float deltaTime) override;
	void onCollisionStart(PhysicsComponent* comp) override;
	void onCollisionEnd(PhysicsComponent* comp) override;
private:
	std::shared_ptr<PhysicsComponent> physComp;
};