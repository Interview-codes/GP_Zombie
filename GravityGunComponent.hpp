#pragma once

#include "Component.hpp"

enum class PullState {
	NoObject,
	Pulling,
	HasObject,
	ThrownObject,
};

class GravityGunComponent : public Component {
public:
	explicit GravityGunComponent(GameObject* gameObject);

	void update(float deltaTime) override;

	bool onKey(SDL_Event& event) override;
	bool onMouse(SDL_Event& event) override;

	void setFollowObject(std::shared_ptr<GameObject> followObject);

	void LookAtPos(glm::vec2 screenPos);
	void pullObject(std::shared_ptr<PhysicsComponent>  comp);
	void throwObject();
	bool hasObject();
	void onCollisionStart(PhysicsComponent* comp) override;

	void onCollisionEnd(PhysicsComponent* comp) override;

	PullState pullState;
	float grapDistance;
	float pullDistance;

	GameObject* grabbedObj;
private:
	float theta;
	bool holding;
	std::shared_ptr<GameObject> followObject;

	float offsetDist = 1.f;

	bool mouseLeftDown = false;
	bool mouseRightDown = false;

};

