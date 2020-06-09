#pragma once
#include "GameObject.hpp"
class ThrowableComponent : public Component {
public:
	explicit ThrowableComponent(GameObject* gameObject);

	void update(float deltaTime) override;
	virtual void onThrow();
	virtual void onFinishThrow();
	virtual void onPull();
	void onCollisionStart(PhysicsComponent* comp) override;
	void onCollisionEnd(PhysicsComponent* comp) override;
	std::shared_ptr<PhysicsComponent> physComp;
protected:
	bool beingThrown = false;
	float endThrowVelocity = 0.3f;
};