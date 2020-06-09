#include <SDL_events.h>
#include <iostream>
#include "SawbladeComponent.hpp"
#include "BarrelComponent.hpp"
#include "GravityGunComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ZombieGame.hpp"
#include "SpriteComponent.hpp"
#include <math.h>

GravityGunComponent::GravityGunComponent(GameObject* gameObject) : Component(gameObject) {
	offsetDist = 12;
	grapDistance = 33;
	pullDistance = 200;
	grabbedObj = nullptr;
	holding = false;
	theta = 0;
	pullState = PullState::NoObject;
}

void GravityGunComponent::update(float deltaTime) {
	int mouseX;
	int mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	LookAtPos(glm::vec2(mouseX, mouseY));
	glm::vec2 dir = glm::rotateZ(glm::vec3(1, 0, 0), glm::radians(gameObject->getRotation()));

	if (theta > 45 && theta < 135) {
		gameObject->setLayer((Layer)(followObject->getLayer() - 1));
	}
	else {
		gameObject->setLayer((Layer)(followObject->getLayer() + 1));
	}
	auto ggPos = followObject->getPosition() + dir * offsetDist;
	ggPos.y -= 5;
	gameObject->setPosition(ggPos);


	if (grabbedObj) {
		if (pullState == PullState::Pulling) {
			auto pc = grabbedObj->getComponent<PhysicsComponent>();
			if (pc) {
				auto dist = glm::distance(grabbedObj->getPosition(), gameObject->getPosition());
				auto pos = (gameObject->getPosition() + dir * dist) / ZombieGame::instance->physicsScale;
				pc->setPosition(pos);
			}
		}

		if (pullState == PullState::HasObject) {
			if (grabbedObj->getComponent<PhysicsComponent>()) {
				auto pos = (gameObject->getPosition() + dir * grapDistance) / ZombieGame::instance->physicsScale;
				grabbedObj->getComponent<PhysicsComponent>()->setPosition(pos);
			}
			else {
				pullState = PullState::NoObject;
			}
		}
	}

	if (mouseRightDown && (pullState == PullState::Pulling || pullState == PullState::NoObject)) {
		auto pos = gameObject->getPosition();
		auto throwComp = ZombieGame::instance->RayCast({ pos.x, pos.y }, { dir.x, dir.y }, pullDistance);
		if (throwComp) {
			pullObject(throwComp->physComp);
		}
		else if (pullState == PullState::Pulling) {
			pullObject(grabbedObj->getComponent<PhysicsComponent>());
		}
	}
	else if (!mouseRightDown) {
		if (pullState == PullState::Pulling || pullState == PullState::ThrownObject) {
			pullState = PullState::NoObject;
		}
	}

	if (mouseLeftDown) {
		throwObject();
	}

}

bool GravityGunComponent::onMouse(SDL_Event& event) {
	if (event.type == SDL_MOUSEMOTION) {
		int mouseX;
		int mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		LookAtPos(glm::vec2(mouseX, mouseY));

	}
	else {
		if (event.button.button == SDL_BUTTON_LEFT) {
			mouseLeftDown = event.type == SDL_MOUSEBUTTONDOWN;
		}
		if (event.button.button == SDL_BUTTON_RIGHT) {
			mouseRightDown = event.type == SDL_MOUSEBUTTONDOWN;
		}
	}
	return false;
}

void GravityGunComponent::LookAtPos(glm::vec2 cursorScreenPos) {

	float screenCenterX = ZombieGame::instance->windowSize.x / 2.0f;
	float screenCenterY = ZombieGame::instance->windowSize.y / 2.0f;

	auto dirVector = glm::normalize(glm::vec2(screenCenterX, screenCenterY) - cursorScreenPos);
	float theta = atan2(dirVector.y, -dirVector.x) * 180 / M_PI;
	GravityGunComponent::theta = theta;
	gameObject->setRotation(theta);
}

bool GravityGunComponent::onKey(SDL_Event& event) {
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t) {

		throwObject();
	}
	return false;
}
void GravityGunComponent::pullObject(std::shared_ptr<PhysicsComponent> comp) {
	if (!hasObject()) {
		if (pullState != PullState::Pulling) Mix_PlayChannel(-1, ZombieGame::instance->gGPickUp, 0);
		pullState = PullState::Pulling;
		auto obj = comp->getGameObject();
		grabbedObj = obj;
		grabbedObj->getComponent<ThrowableComponent>()->onPull();
		glm::vec2 dir = -glm::rotateZ(glm::vec3(1, 0, 0), glm::radians(gameObject->getRotation()));
		comp->setLinearVelocity(2.0f * dir);

		if (glm::distance(obj->getPosition(), gameObject->getPosition()) < grapDistance) {
			pullState = PullState::HasObject;
			comp->setLinearVelocity({ 0, 0 });
		}
	}
}

bool GravityGunComponent::hasObject() {
	return pullState == PullState::HasObject;
}

void GravityGunComponent::throwObject() {
	if (hasObject()) {
		pullState = PullState::ThrownObject;
		float throwSpeed = 3.0f;
		auto gOPhys = grabbedObj->getComponent<PhysicsComponent>();

		grabbedObj->getComponent<ThrowableComponent>()->onThrow();
		glm::vec2 throwVec = glm::rotateZ(glm::vec3(1, 0, 0), glm::radians(gameObject->getRotation())) * throwSpeed;
		gOPhys->setLinearVelocity(throwVec);
		Mix_PlayChannel(-1, ZombieGame::instance->gGShoot, 0);
	}
}

void GravityGunComponent::setFollowObject(std::shared_ptr<GameObject> followObject) {
	this->followObject = followObject;
}

void GravityGunComponent::onCollisionStart(PhysicsComponent* comp) {

}

void GravityGunComponent::onCollisionEnd(PhysicsComponent* comp) {

}


