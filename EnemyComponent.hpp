#pragma once
#include "GameObject.hpp"
#include "Damageable.hpp"
#include "PlayerController.hpp"

class EnemyComponent : public Component, public Damageable {
public:
	explicit EnemyComponent(GameObject* gameObject);

	void update(float deltaTime) override;

	void onCollisionStart(PhysicsComponent* comp) override;

	void onCollisionEnd(PhysicsComponent* comp) override;

	void setTarget(std::shared_ptr<GameObject> gameobj);

	int getHealth();

	void setHealth(float healthValue);

	bool isAlive() override;

	void applyDamage(float damageAmount) override;

	void onDeath() override;
private:
	float health;
	float movementSpeed;

	void HandleAnimation(float deltaTime);
	std::shared_ptr<PhysicsComponent> physComp;

	void playRandomAlertSound();
	void playRandomAttackSound();
	void playRandomPainSound();
	void playRandomDeathSound();
	void playRandomSprintSound();

	GameObject* gameObject;
	bool createMedkit = false;

	glm::vec2 idleDirection = { 0.f, 1.f };
	const float idleTime = 3.f;
	float idleTimer = 0.f;
	const float idleMovementSpeedModifier = 0.5f;
	const float sprintMovementSpeedModifier = 1.3f;

	const float attackTime = 1.f;
	float attackTimer = 0.f;

	const float dieTime = 8.f;
	float dieTimer = 0.f;

	bool playerUnknown = true;
	bool indicateSprint = false;

	// ranges - above
	const float longRange = 450.f;
	const float mediumRange = 350.f;
	const float sprintRange = 175.f;
	const float closeRange = 55.f;
	std::shared_ptr<PlayerController> pc;
	std::shared_ptr<GameObject> targetGameObject;
};