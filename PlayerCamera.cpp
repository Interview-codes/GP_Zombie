#include "PlayerCamera.hpp"
#include "ZombieGame.hpp"

using namespace glm;


PlayerCamera::PlayerCamera(GameObject *gameObject) : Component(gameObject)
{
    camera.setOrthographicProjection(ZombieGame::windowSize.y/2,-1,1);
}

sre::Camera &PlayerCamera::getCamera() {
    return camera;
}

void PlayerCamera::update(float deltaTime) {
	auto position = followObject->getPosition();
    gameObject->setPosition(position);
    vec3 eye (position, 0);
    vec3 at (position, -1);
    vec3 up (0, 1, 0);
    camera.lookAt(eye, at, up);
}

void PlayerCamera::setFollowObject(std::shared_ptr<GameObject> followObject) {
    this->followObject = followObject;
}
