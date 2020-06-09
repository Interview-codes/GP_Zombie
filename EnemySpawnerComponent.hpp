#pragma once
#include "GameObject.hpp"
class EnemySpawnerComponent : public Component {
public:
	explicit EnemySpawnerComponent(GameObject* gameObject);

	void setTimer(float time);

	void enable(bool value);
	bool isEnabled();

	void update(float deltaTime) override;

	void onCollisionStart(PhysicsComponent* comp) override;

	void onCollisionEnd(PhysicsComponent* comp) override;
private:
	bool enabled = false;
	bool deleteOnFinish = true;
	float spawnTime = 10.f; // set globally
	float spawnTimer = 0.f;

	void spawnEnemy();

	int spawnCount = 50; // set globally
	int spawnCounter = 0;
};