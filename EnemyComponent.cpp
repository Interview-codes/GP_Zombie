#include <SDL_events.h>
#include <iostream>
#include "EnemyComponent.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ZombieGame.hpp"
#include "CharacterAnimationComponent.hpp"
#include <math.h>

EnemyComponent::EnemyComponent(GameObject* gameObject) : Component(gameObject) {
	physComp = gameObject->getComponent<PhysicsComponent>();
	this->gameObject = gameObject;
	health = 25.f;
	movementSpeed = 40.f;
}



void EnemyComponent::update(float deltaTime) {
	if (health <= 0) {
		onDeath();
		dieTimer -= deltaTime;
		if (dieTimer <= 0) ZombieGame::instance->deleteGameObject(gameObject);
		return;
	}

	HandleAnimation(deltaTime);

	// damping
	glm::vec2 vel = physComp->getLinearVelocity() * 0.3f;
	if (glm::length(vel) < 0.001f) vel = glm::vec2(0, 0);
	physComp->setLinearVelocity(vel);

	if (!pc->isAlive()) return;

	float distTarget = glm::distance(targetGameObject->getPosition(), gameObject->getPosition());
	if (pc->isDebug) distTarget = longRange;

	// close range
	if (distTarget <= closeRange) {
		glm::vec2 dir = glm::normalize(targetGameObject->getPosition() - gameObject->getPosition());
		//physComp->addForce(dir * movementSpeed * 0.3f * deltaTime);
		if (attackTimer >= attackTime) {
			// std::cout << "Zombie " << gameObject->getUid() << " attacking" << std::endl;
			gameObject->getComponent<CharacterAnimationComponent>()->playAttackAnimation(attackTime, targetGameObject->getPosition(), false);
			playRandomAttackSound();
			pc->applyDamage(3);
			attackTimer = 0;
		}
		attackTimer += deltaTime;
	}
	else {
		gameObject->getComponent<CharacterAnimationComponent>()->playWalkAnimation(true);
		attackTimer = attackTime / 2;
	}

	// sprint
	if (distTarget > closeRange && distTarget <= sprintRange)
	{
		if (indicateSprint) {
			playRandomSprintSound();
			indicateSprint = false;
		}
		glm::vec2 dir = glm::normalize(targetGameObject->getPosition() - gameObject->getPosition());
		physComp->addForce(dir * movementSpeed * sprintMovementSpeedModifier * deltaTime);
	}

	// medium range
	if (distTarget > sprintRange && distTarget <= mediumRange) {
		if (playerUnknown) {
			playRandomAlertSound();
			playerUnknown = false;
		}
		indicateSprint = true;
		glm::vec2 dir = glm::normalize(targetGameObject->getPosition() - gameObject->getPosition());
		physComp->addForce(dir * movementSpeed * deltaTime);
	}

	// long range
	if (distTarget > mediumRange) {
		playerUnknown = true;
		idleTimer += deltaTime;

		if (ZombieGame::instance->isOutsideMap(gameObject->getPosition())) {
			//std::cout << "Zombie [long distance] " << gameObject->getUid() << " is outside of map!" << std::endl;
			glm::vec2 mapCenter = ZombieGame::TranslateIsoToWorld(ZombieGame::instance->getMapDimensions() / 2.f);
			glm::vec2 dir = glm::normalize(mapCenter - gameObject->getPosition());
			idleDirection = dir;
		}
		else if (idleTimer >= idleTime) {
			float theta = rand() % 360;
			glm::vec2 rot{ glm::cos(glm::radians(theta)), glm::sin(glm::radians(theta)) };
			idleDirection = glm::normalize(rot);
			idleTimer = 0.f;
		}
		physComp->addForce(idleDirection * (movementSpeed * idleMovementSpeedModifier) * deltaTime);
	}
}


void EnemyComponent::playRandomAlertSound() {
	int num = rand() % 3;
	switch (num) {
	case 0:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieAlert1, 0);
		return;
	case 1:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieAlert2, 0);
		return;
	case 2:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieAlert3, 0);
		return;
	}
}

void EnemyComponent::playRandomPainSound() {
	int num = rand() % 5;
	switch (num) {
	case 0:
		Mix_PlayChannel(-1, ZombieGame::instance->zombiePain1, 0);
		return;
	case 1:
		Mix_PlayChannel(-1, ZombieGame::instance->zombiePain2, 0);
		return;
	case 2:
		Mix_PlayChannel(-1, ZombieGame::instance->zombiePain3, 0);
		return;
	case 3:
		Mix_PlayChannel(-1, ZombieGame::instance->zombiePain4, 0);
		return;
	case 4:
		Mix_PlayChannel(-1, ZombieGame::instance->zombiePain5, 0);
		return;
	}
}

void EnemyComponent::playRandomAttackSound() {
	int num = rand() % 3;
	switch (num) {
	case 0:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieClawStrike1, 0);
		return;
	case 1:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieClawStrike2, 0);
		return;
	case 2:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieClawStrike3, 0);
		return;
	}
}

void EnemyComponent::playRandomDeathSound() {
	int num = rand() % 3;
	switch (num) {
	case 0:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieDie1, 0);
		return;
	case 1:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieDie2, 0);
		return;
	case 2:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieDie3, 0);
		return;
	}
}

void EnemyComponent::playRandomSprintSound() {
	int num = rand() % 6;
	switch (num) {
	case 0:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieSprint1, 0);
		return;
	case 1:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieSprint2, 0);
		return;
	case 2:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieSprint3, 0);
		return;
	case 3:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieSprint4, 0);
		return;
	case 4:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieSprint5, 0);
		return;
	case 5:
		Mix_PlayChannel(-1, ZombieGame::instance->zombieSprint6, 0);
		return;
	}
}

void EnemyComponent::HandleAnimation(float deltaTime) {

}

bool EnemyComponent::isAlive() {
	return !dead;
}

void EnemyComponent::onDeath() {
	if (!isAlive()) return;
	dieTimer = dieTime;
	dead = true;
	int num = rand() % 8;

	if (num <= 1) {
		Mix_PlayChannel(-1, ZombieGame::instance->warning, 0);
		ZombieGame::instance->createWorldObject(gameObject->getPosition(), WorldObjectTypes::Medkit);
	}

	physComp->setLinearVelocity(glm::vec2(0, 0));
	physComp->setSensor(true);

	gameObject->setLayer(Layer::TileTexture);

	playRandomDeathSound();
	//ZombieGame::instance->deleteGameObject(gameObject);
	ZombieGame::instance->incrementScore(10);
	ZombieGame::instance->decrementEnemyCount();
	gameObject->getComponent<CharacterAnimationComponent>()->playDieAnimation(false);
}

int EnemyComponent::getHealth() {
	return health;
}

void EnemyComponent::setHealth(float healthValue) {
	health = healthValue;
}

void EnemyComponent::applyDamage(float damageAmount) {
	health -= damageAmount;
	// std::cout << "Applied damage: " << damageAmount << std::endl; // DEBUG
	if (health > 0) playRandomPainSound();
}


void EnemyComponent::setTarget(std::shared_ptr<GameObject> gameobj) {
	targetGameObject = gameobj;
	pc = targetGameObject->getComponent<PlayerController>();
}

void EnemyComponent::onCollisionStart(PhysicsComponent* comp) {
	idleTimer = idleTime;
}

void EnemyComponent::onCollisionEnd(PhysicsComponent* comp) {

}