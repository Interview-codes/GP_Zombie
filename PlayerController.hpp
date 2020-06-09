#pragma once

#include "Component.hpp"
#include "Damageable.hpp"

class PlayerController : public Component, public Damageable {
public:
	explicit PlayerController(GameObject* gameObject);

	void update(float deltaTime) override;

	bool onKey(SDL_Event& event) override;

	void onCollisionStart(PhysicsComponent* comp) override;

	void onCollisionEnd(PhysicsComponent* comp) override;

	void setGravityGun(std::shared_ptr<GameObject> gameObject);

	int getHealth();

	int getSuitEnergy();

	void setHealth(float healthValue);

	void applyDamage(float damageAmount);

	void onDeath() override;

	bool isAlive() override;

	std::shared_ptr<GameObject> gravityGun;

	bool isDebug = false;

	static const int maxHealth = 100;

private:
	int health = maxHealth;
	float suitEnergy = 100.f;
	const float suitEnergyCap = suitEnergy;
	bool sprinting = false;
	float sprintSpeedModifier = 2.3f;
	float energyDrainPerSecond = 15.f;
	float speed;
	std::shared_ptr<PhysicsComponent> physComp;

	void playRandomPainSound();
	void onSprint();
	void onStopSprint();
	void onSprintExhausted();

	bool dead = false;
	bool up = false;
	bool right = false;
	bool down = false;
	bool left = false;
};

