#pragma once

#include "sre/Camera.hpp"
#include "glm/glm.hpp"
#include "Component.hpp"

class PlayerCamera : public Component {
public:
	explicit PlayerCamera(GameObject* gameObject);

	void update(float deltaTime) override;

	void setFollowObject(std::shared_ptr<GameObject> followObject); // removed offset

	sre::Camera& getCamera();
private:
	sre::Camera camera;
	std::shared_ptr<GameObject> followObject;
};

