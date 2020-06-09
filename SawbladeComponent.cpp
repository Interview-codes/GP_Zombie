#include <SDL_events.h>
#include <iostream>
#include "SawbladeComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ZombieGame.hpp"
#include "SpriteComponent.hpp"
#include <math.h>
#include "GravityGunComponent.hpp"
#include "PlayerController.hpp"
#include "EnemyComponent.hpp"

SawbladeComponent::SawbladeComponent(GameObject* gameObject) : ThrowableComponent(gameObject) {
	physComp = ThrowableComponent::physComp;
	initThrowVel = 2.f;
	angVelo = 0;
}
void SawbladeComponent::update(float deltaTime) {
	ThrowableComponent::update(deltaTime);
	gameObject->setRotation(gameObject->getRotation() + angVelo * deltaTime);
	if (physComp->isSensor() && !physComp->isColliding()) physComp->setSensor(false);
}

void SawbladeComponent::onPull() {
	ThrowableComponent::onPull();
	slicedObjects.clear();
	setRotation(false);
}
void SawbladeComponent::onThrow() {
	ThrowableComponent::onThrow();
	setRotation(true);
}
void SawbladeComponent::onFinishThrow() {
	ThrowableComponent::onFinishThrow();
	setRotation(false);
	slicedObjects.clear();
}
void SawbladeComponent::setRotation(bool t) {
	if (t) {
		angVelo = -4 * 180;
	}
	else {
		gameObject->setRotation(0);
		angVelo = 0;
	}
}
void SawbladeComponent::onCollisionStart(PhysicsComponent* comp) {
	ThrowableComponent::onCollisionStart(comp);
	if (!beingThrown) {
		return;
	}


	glm::vec2 currentVel = physComp->getLinearVelocity();
	auto compGameObject = comp->getGameObject();
	long gameObjectuid = compGameObject->getUid();

	auto enemyComponent = compGameObject->getComponent<EnemyComponent>();
	if (enemyComponent != nullptr) {

		// If velocity is too low, return
		if (glm::length(currentVel) <= 0.3f) {
			return;
		}

		// if enemy already sliced, return
		bool alreadySliced = slicedObjects.find(gameObjectuid) != slicedObjects.end();
		if (alreadySliced) {
			return;
		}

		Mix_PlayChannel(-1, ZombieGame::instance->sawbladeSlice, 0);
		Mix_PlayChannel(-1, ZombieGame::instance->sawbladeSlice2, 0);

		if (beingThrown) physComp->setSensor(true);
		
		auto damage = glm::length(physComp->getLinearVelocity()) * 10;
		enemyComponent->applyDamage(damage);
		
		physComp->addImpulse(-.02f * glm::normalize(currentVel)); // decrease speed of sawblade

		// mark object as sliced
		slicedObjects.insert(gameObjectuid); 
	}
	else {

		// If we hit an object that is not enemy, stick to it!
		if (comp->isSensor() == false && glm::length(currentVel) >= 0.3f) {
			Mix_PlayChannel(-1, ZombieGame::instance->sawbladeStick, 0);
			physComp->setLinearVelocity({ 0,0 });
		}
	}
}
void SawbladeComponent::onCollisionEnd(PhysicsComponent* comp) {
	ThrowableComponent::onCollisionEnd(comp);
}