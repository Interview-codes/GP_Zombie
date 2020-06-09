#pragma once
#include "SpriteAnimationComponent.hpp"

class CharacterAnimationComponent : public SpriteAnimationComponent {
public:
	enum class Direction {
		Up, Right, Down, Left
	};

	enum class Type {
		Player, Light_Female, Light_Male, Dark_Female, Dark_Male
	};

	enum class Animation {
		Walk, Attack, Die
	};

	CharacterAnimationComponent(GameObject* gameObject);

	void update(float deltaTime) override;

	void setDirection(Direction direction);

	void setUpAnimation(Type type);

	bool lookAtMouse = false;

	void playAttackAnimation(float animationTime, glm::vec2 point, bool loop);
	void playWalkAnimation(bool loop);
	void playDieAnimation(bool loop);

private:

	void updateDirection();
	glm::vec2 directionPoint;

	Animation playingAnimation = Animation::Walk;

	void updateWalkAnimation(float deltaTime);
	void updateAttackAnimation();
	void updateDieAnimation();

	bool playedAnimation = false;

	std::string getTileString(int id);

	Direction dir = Direction::Left;

	std::shared_ptr<PhysicsComponent> physComp;

	std::string getCharecterString(Type type);

	std::vector<sre::Sprite> upAnimation;
	std::vector<sre::Sprite> rightAnimation;
	std::vector<sre::Sprite> downAnimation;
	std::vector<sre::Sprite> leftAnimation;

	std::vector<sre::Sprite> upAttackAnimation;
	std::vector<sre::Sprite> rightAttackAnimation;
	std::vector<sre::Sprite> downAttackAnimation;
	std::vector<sre::Sprite> leftAttackAnimation;

	std::vector<sre::Sprite> dieAnimation;

};


