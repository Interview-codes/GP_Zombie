#include <SDL_events.h>
#include <iostream>
#include "MedkitComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ZombieGame.hpp"
#include "PlayerController.hpp"
#include <math.h>

MedkitComponent::MedkitComponent(GameObject* gameObject) : Component(gameObject) {
	physComp = gameObject->getComponent<PhysicsComponent>();
}

void MedkitComponent::update(float deltaTime) {
}

void MedkitComponent::onCollisionStart(PhysicsComponent* comp) {
	auto player = comp->getGameObject()->getComponent<PlayerController>();
	if (player) {
		int playerHealth = player->getHealth();
		if (playerHealth < PlayerController::maxHealth) {
			Mix_PlayChannel(-1, ZombieGame::instance->smallmedkit1, 0);
			player->setHealth(playerHealth + 3);
			ZombieGame::instance->deleteGameObject(gameObject);
		}

	}
}
void MedkitComponent::onCollisionEnd(PhysicsComponent* comp) {
}