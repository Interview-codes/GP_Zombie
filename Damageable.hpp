#pragma once


class Damageable {
public:
	virtual int getHealth() = 0;
	virtual void setHealth(float healthValue) = 0;
	virtual void applyDamage(float damageAmount) = 0;

	virtual void onDeath() = 0;
	virtual bool isAlive() = 0;

	bool dead = false;
	static const float maxHealth;
	float health;


};