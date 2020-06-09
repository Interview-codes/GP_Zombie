#pragma once
#include "GameObject.hpp"
#include "ThrowableComponent.hpp"
#include "SDL_mixer.h"
#include <set>
class SawbladeComponent : public ThrowableComponent {
public:
	explicit SawbladeComponent(GameObject* gameObject);
	void update(float deltaTime) override;
	void onPull();
	void onThrow();
	void onFinishThrow();
	void onCollisionStart(PhysicsComponent* comp) override;
	void onCollisionEnd(PhysicsComponent* comp) override;
private:
	void setRotation(bool t);
	std::set<long> slicedObjects;
	float angVelo;
	float initThrowVel;
	std::shared_ptr<PhysicsComponent> physComp;

};