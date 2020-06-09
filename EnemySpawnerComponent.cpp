#include <SDL_events.h>
#include <iostream>
#include "EnemySpawnerComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ZombieGame.hpp"
#include "SpriteComponent.hpp"
#include <math.h>
#include <ctime>

EnemySpawnerComponent::EnemySpawnerComponent(GameObject* gameObject) : Component(gameObject) {
	enabled = true;
	spawnCount = ZombieGame::instance->spawnCount;
	spawnTime = ZombieGame::instance->spawnRate;
}

void EnemySpawnerComponent::update(float deltaTime) {
	if (!enabled) {
		Mix_PlayChannel(-1, ZombieGame::instance->zombieSpawnSound, 0);
		if(deleteOnFinish) ZombieGame::instance->deleteGameObject(gameObject);
		return;
	}

	if(spawnTimer <= spawnTime) spawnTimer += deltaTime;

	if (spawnTimer >= spawnTime) {
		spawnTimer = 0.f;
		spawnEnemy();
		if (spawnCounter >= spawnCount) enabled = false;
	}
}

void EnemySpawnerComponent::spawnEnemy() {
	spawnCounter++;
	
	glm::vec2 pos = gameObject->getPosition(); // + ZombieGame::TranslateIsoToWorld(glm::vec2(0, 0));
	auto ref = ZombieGame::instance->createWorldObject(pos, WorldObjectTypes::Enemy);
}


void EnemySpawnerComponent::setTimer(float time) {
	spawnTime = time;
	spawnTimer = 0.f;
}

bool EnemySpawnerComponent::isEnabled() {
	return enabled;
}

void EnemySpawnerComponent::enable(bool value) {
	enabled = value;
}

void EnemySpawnerComponent::onCollisionStart(PhysicsComponent* comp) {
}

void EnemySpawnerComponent::onCollisionEnd(PhysicsComponent* comp) {
}