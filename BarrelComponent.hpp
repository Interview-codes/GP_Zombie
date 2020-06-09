#pragma once
#include "GameObject.hpp"
#include "ThrowableComponent.hpp"
#include "SDL_mixer.h"
class BarrelComponent : public ThrowableComponent {
public:
	explicit BarrelComponent(GameObject* gameObject);
	void update(float deltaTime) override;
	void onPull();
	void onThrow();
	void onFinishThrow();
	void onCollisionStart(PhysicsComponent* comp) override;
	void onCollisionEnd(PhysicsComponent* comp) override;
private:
	float enemyKnockbackForce = 30.f;
	float angVelo;
	float initThrowVel;
	std::shared_ptr<PhysicsComponent> physComp;
};