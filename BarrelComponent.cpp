#include <SDL_events.h>
#include <iostream>
#include "BarrelComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ZombieGame.hpp"
#include "SpriteComponent.hpp"
#include "EnemyComponent.hpp"
#include <math.h>


BarrelComponent::BarrelComponent(GameObject* gameObject) : ThrowableComponent(gameObject) {
	physComp = gameObject->getComponent<PhysicsComponent>();
	initThrowVel = 0.8f;
}

void BarrelComponent::update(float deltaTime) {
	ThrowableComponent::update(deltaTime);
}
void BarrelComponent::onPull() {
	ThrowableComponent::onPull();
}
void BarrelComponent::onThrow() {
	ThrowableComponent::onThrow();
}
void BarrelComponent::onFinishThrow() {
}
void BarrelComponent::onCollisionStart(PhysicsComponent* comp) {
	ThrowableComponent::onCollisionStart(comp);

	

	auto hitGameObject = comp->getGameObject();

	float currentVelocity = glm::length(physComp->getLinearVelocity());

	auto enemyComp = hitGameObject->getComponent<EnemyComponent>();
	if (enemyComp != nullptr) {
		if (comp->isSensor()) return;
		// apply knockback to enemy + damage if barrel is fast enough
		if (beingThrown && currentVelocity > 1.f) {
			Mix_PlayChannel(-1, ZombieGame::instance->barrelImpact, 0);
			glm::vec2 dir = glm::normalize(hitGameObject->getPosition() - gameObject->getPosition());
			comp->addForce(dir * enemyKnockbackForce);

			auto damage = glm::length(physComp->getLinearVelocity()) * 10;
			enemyComp->applyDamage(damage);
			physComp->setLinearVelocity({ 0,0 });
		}
		else {
			// soft impact
			Mix_PlayChannel(-1, ZombieGame::instance->barrelImpact3, 0);
		}

	}
	else {
		if (currentVelocity > .3f) {
			Mix_PlayChannel(-1, ZombieGame::instance->barrelImpact2, 0);
		}
	}
}
void BarrelComponent::onCollisionEnd(PhysicsComponent* comp) {
}