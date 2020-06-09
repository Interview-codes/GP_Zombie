#include <SDL_events.h>
#include <iostream>
#include "PlayerController.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ThrowableComponent.hpp"
#include "EnemyComponent.hpp"
#include "MedkitComponent.hpp"
#include "ZombieGame.hpp"
#include <math.h>

PlayerController::PlayerController(GameObject *gameObject) : Component(gameObject), Damageable() {
	speed = 205.f;
	physComp = gameObject->getComponent<PhysicsComponent>();
}

void PlayerController::setGravityGun(std::shared_ptr<GameObject> gameObject) {
	gravityGun = gameObject;
}

void PlayerController::onDeath() {
	dead = true;
	//std::cout << "Player has died" << std::endl;
	Mix_PlayChannel(-1, ZombieGame::instance->flatline, 0);
	ZombieGame::instance->setGameState(GameState::GameOver);
}

int PlayerController::getHealth() {
	return health;
}

void PlayerController::setHealth(float healthValue) {
	health = healthValue;
}

bool PlayerController::isAlive() {
	return !dead;
}

int PlayerController::getSuitEnergy() {
	return suitEnergy;
}

void PlayerController::applyDamage(float damageAmount) {
	if (dead) return;

	playRandomPainSound();
	if(health > 0) health -= damageAmount;
	if (health < 0) health = 0;
	//std::cout << "Player HP: " << health << std::endl;

	if (health <= 0 && !dead) {
		onDeath();
	}
}

void PlayerController::playRandomPainSound() {
	int num = rand() % 3;
	switch (num) {
	case 0:
		Mix_PlayChannel(-1, ZombieGame::instance->playerPain1, 0);
		return;
	case 1:
		Mix_PlayChannel(-1, ZombieGame::instance->playerPain2, 0);
		return;
	case 2:
		Mix_PlayChannel(-1, ZombieGame::instance->playerPain3, 0);
		return;
	}
}

void PlayerController::onSprint() {
	Mix_PlayChannel(-1, ZombieGame::instance->playerSprint, 0);
}

void PlayerController::onStopSprint() {
	//std::cout << "stopped sprinting" << std::endl;
}

void PlayerController::onSprintExhausted() {
	std::cout << "exhausted sprinting" << std::endl;
	// play sound
}

void PlayerController::update(float deltaTime) {
	glm::vec2 vel = physComp->getLinearVelocity() * 0.3f;
	if (glm::length(vel) < 0.001f) vel = glm::vec2(0, 0);
	physComp->setLinearVelocity(vel);

	if (!sprinting && suitEnergy < suitEnergyCap) suitEnergy += (energyDrainPerSecond*1.1f) * deltaTime;

	auto addDir = glm::vec2(0, 0);
	if(this->left) {
		addDir += glm::vec2(-1.0f, 0.0f);
	}
	if (this->right) {
		addDir += glm::vec2(1.0f, 0.0f);
	}
	if (this->up) {
		addDir += glm::vec2(0.0f, 1.0f);
	}
	if (this->down) {
		addDir += glm::vec2(0.0f, -1.0f);
	}

	if (glm::length(addDir)) {
		float sprintSpeed;

		if (suitEnergy > 0) {
			sprintSpeed = sprinting ? sprintSpeedModifier : 1.f;
		}
		else {
			sprintSpeed = 1.f;
		}

		physComp->addForce(glm::normalize(addDir) * speed * sprintSpeed * deltaTime);

		if (sprinting && suitEnergy > 0) {
			suitEnergy -= energyDrainPerSecond * deltaTime;
			if (suitEnergy <= 0) {
				suitEnergy = 0;
				onSprintExhausted();
			}
		}
	}

}

bool PlayerController::onKey(SDL_Event &event) {
	if (event.key.keysym.sym == SDLK_d) {
		right = event.type == SDL_KEYDOWN;
	}
	if (event.key.keysym.sym == SDLK_a) {
		left = event.type == SDL_KEYDOWN;
	}
	if (event.key.keysym.sym == SDLK_w) {
		up = event.type == SDL_KEYDOWN;
	}
	if (event.key.keysym.sym == SDLK_s) {
		down = event.type == SDL_KEYDOWN;
	}
	if (event.key.keysym.sym == SDLK_b) {
		glm::vec2 pos = ZombieGame::instance->TranslateWorldToIso(gameObject->getPosition());
		std::cout << "x:" << pos.x << " y:" << pos.y << std::endl;
	}
	if (event.key.keysym.sym == SDLK_LSHIFT) {
		bool wasSprinting = sprinting;
		sprinting = event.type == SDL_KEYDOWN;

		if (!wasSprinting && sprinting) {
			onSprint();
		}
		else if (wasSprinting && !sprinting) {
			onStopSprint();
		}
	}
    return false;
}

void PlayerController::onCollisionStart(PhysicsComponent *comp) {
	/*
	auto enemyComp = comp->getGameObject()->getComponent<EnemyComponent>();
	
	if (enemyComp != nullptr) {
		enemyComp->applyDamage(10);
	}
	*/
}

void PlayerController::onCollisionEnd(PhysicsComponent *comp) {

}


