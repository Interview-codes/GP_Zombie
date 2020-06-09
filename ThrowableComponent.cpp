#include <SDL_events.h>
#include <iostream>
#include "ThrowableComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ZombieGame.hpp"
#include "SpriteComponent.hpp"
#include <math.h>
#include "Tile.hpp"

ThrowableComponent::ThrowableComponent(GameObject* gameObject) : Component(gameObject) {
	physComp = gameObject->getComponent<PhysicsComponent>();
}
void ThrowableComponent::onPull() {
	beingThrown = false;
}

void ThrowableComponent::onThrow() {
	beingThrown = true;
}

void ThrowableComponent::onFinishThrow() {
}

void ThrowableComponent::update(float deltaTime) {
	auto playerPos = ZombieGame::instance->playerGameObject->getPosition();
	auto pos = gameObject->getPosition();
	auto dist = glm::distance(pos, playerPos);
	auto vel = physComp->getLinearVelocity();

	if (dist > (float)Tile::TILE_WIDTH) {
		physComp->setDamping(0.97f);
	}
	else {
		physComp->setDamping(0.99f);
	}

	if (beingThrown && glm::length(physComp->getLinearVelocity()) < endThrowVelocity ) {
		beingThrown = false;
		onFinishThrow();
	}


}

void ThrowableComponent::onCollisionStart(PhysicsComponent* comp) {
}
void ThrowableComponent::onCollisionEnd(PhysicsComponent* comp) {

}