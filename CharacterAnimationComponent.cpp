#include "CharacterAnimationComponent.hpp"
#include "GameObject.hpp"
#include "ZombieGame.hpp"
#include <memory>

CharacterAnimationComponent::CharacterAnimationComponent(GameObject* gameObject) : SpriteAnimationComponent(gameObject) {
	physComp = gameObject->getComponent<PhysicsComponent>();
}

void CharacterAnimationComponent::update(float deltaTime) {
	updateDirection();

	switch (playingAnimation) {
	case Animation::Attack:
		updateAttackAnimation();
		break;
	case Animation::Walk:
		updateWalkAnimation(deltaTime);
		break;
	case Animation::Die:
		updateDieAnimation();
		break;
	};

	SpriteAnimationComponent::update(deltaTime);
}

void CharacterAnimationComponent::playAttackAnimation(float animationTime, glm::vec2 point, bool loop) {
	playingAnimation = Animation::Attack;
	directionPoint = point;
	setAnimationTime(animationTime / 5);
	setLoop(loop);
	if(!loop) resetSpriteAnimation();
}

void CharacterAnimationComponent::playWalkAnimation(bool loop) {
	playingAnimation = Animation::Walk;
	setLoop(loop);
	if(!loop) resetSpriteAnimation();
}

void CharacterAnimationComponent::playDieAnimation(bool loop) {
	playingAnimation = Animation::Die;
	setLoop(loop);
	if(!loop) resetSpriteAnimation();
}

void CharacterAnimationComponent::updateDieAnimation() {
	setSprites(dieAnimation);
}

void CharacterAnimationComponent::updateAttackAnimation() {
	if (leftAttackAnimation.size() == 0) return;
	playedAnimation = true;
	switch (dir) {
	case Direction::Left:
		setSprites(leftAttackAnimation);
		break;
	case Direction::Up:
		setSprites(upAttackAnimation);
		break;
	case Direction::Right:
		setSprites(rightAttackAnimation);
		break;
	case Direction::Down:
		setSprites(upAttackAnimation);
		break;
	}
}

void CharacterAnimationComponent::updateWalkAnimation(float deltaTime) {
	switch (dir) {
	case Direction::Left:
		setSprites(leftAnimation);
		break;
	case Direction::Up:
		setSprites(upAnimation);
		break;
	case Direction::Right:
		setSprites(rightAnimation);
		break;
	case Direction::Down:
		setSprites(downAnimation);
		break;
	}

	// Update AnimationTime (Should always happen)
	auto vel = physComp->getLinearVelocity();
	if (vel != glm::vec2(0, 0)) {
		setAnimationTime(1.0f / (glm::length(vel) / 2 / deltaTime));
	}
	else {
		resetSpriteAnimation();
		setAnimationTime(0);
	}
}

void CharacterAnimationComponent::updateDirection() {
	glm::vec2 dirVector;
	if (!lookAtMouse) {
		if (playingAnimation == Animation::Attack) {
			dirVector = glm::normalize(gameObject->getPosition() - directionPoint);
			dirVector.x = -dirVector.x;
		}
		else
		{
			auto vel = physComp->getLinearVelocity();
			dirVector = glm::normalize(vel);
		}
	}
	else 
	{
		int mouseX;
		int mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		float screenCenterX = ZombieGame::instance->windowSize.x / 2.0f;
		float screenCenterY = ZombieGame::instance->windowSize.y / 2.0f;

		dirVector = glm::normalize(glm::vec2(screenCenterX, screenCenterY) - glm::vec2(mouseX, mouseY));
		dirVector.x = -dirVector.x;
	}

	auto theta = atan2(dirVector.y, dirVector.x) * 180 / M_PI;
	if (theta > -45 && theta < 45) {
		dir = Direction::Right;
	}
	else if (theta > 45 && theta < 135) {
		dir = Direction::Up;
	}
	else if (theta > -135 && theta < -45) {
		dir = Direction::Down;
	}
	else if (theta > 135 || theta < -45) {
		dir = Direction::Left;
	}
}

void CharacterAnimationComponent::setDirection(Direction direction) {
	dir = direction;
}

std::string CharacterAnimationComponent::getTileString(int id) {
	auto idString = std::to_string(id);
	std::string tileString = "tile";
	for (int i = 0; i < 3 - idString.size(); i++) {
		tileString += "0";
	}
	return tileString + idString;
}

std::string CharacterAnimationComponent::getCharecterString(Type type) {
	std::string charecterString;
	switch (type) {
	case Type::Player:
		charecterString = "Player";
		break;
	case Type::Dark_Female:
		charecterString = "Zombie_Dark_Female";
		break;
	case Type::Dark_Male:
		charecterString = "Zombie_Dark_Male";
		break;
	case Type::Light_Female:
		charecterString = "Zombie_Light_Female";
		break;
	case Type::Light_Male:
		charecterString = "Zombie_Light_Male";
		break;
	}
	return charecterString;
}

void CharacterAnimationComponent::setUpAnimation(Type type) {
	auto spriteAtlas = ZombieGame::instance->spriteZombies;

	std::string charecterString = getCharecterString(type);

	for(int i = 0; i < 36; i++){
		auto walkSwitch = (int)floor(i / 9);
		auto hitSwitch = (int)floor(i / 6);
		auto deadSwitch = (int)floor(i / 6);
		auto tileString = getTileString(i);
		switch (walkSwitch) {
		case 0:
			upAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/" + tileString + ".png"));
			break;
		case 1:
			leftAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/" + tileString + ".png"));
			break;
		case 2:
			downAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/" + tileString + ".png"));
			break;
		case 3:
			rightAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/" + tileString + ".png"));
			break;
		}
		if (type != Type::Player) { // The player does not have an attack animation
			switch (hitSwitch) {
			case 0:
				upAttackAnimation.push_back(spriteAtlas->get(charecterString + "/Hit/" + tileString + ".png"));
				break;
			case 1:
				leftAttackAnimation.push_back(spriteAtlas->get(charecterString + "/Hit/" + tileString + ".png"));
				break;
			case 2:
				downAttackAnimation.push_back(spriteAtlas->get(charecterString + "/Hit/" + tileString + ".png"));
				break;
			case 3:
				rightAttackAnimation.push_back(spriteAtlas->get(charecterString + "/Hit/" + tileString + ".png"));
				break;
			}
		}
		if (type != Type::Player) { // The player does not have a die animation
			switch (deadSwitch) {
			case 0:
				dieAnimation.push_back(spriteAtlas->get(charecterString + "/Dead/" + tileString + ".png"));
				break;
			}
		}
	}

	/*
	for (int i = 0; i < 9; i++) {
		upAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/tile00" + std::to_string(i) + ".png"));
	}

	leftAnimation = std::vector<sre::Sprite>();
	leftAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/tile009" + ".png"));
	for (int i = 10; i < 18; i++) {
		leftAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/tile0" + std::to_string(i) + ".png"));
	}

	downAnimation = std::vector<sre::Sprite>();
	for (int i = 18; i < 27; i++) {
		downAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/tile0" + std::to_string(i) + ".png"));
	}

	rightAnimation = std::vector<sre::Sprite>();
	for (int i = 27; i < 36; i++) {
		rightAnimation.push_back(spriteAtlas->get(charecterString + "/Walk/tile0" + std::to_string(i) + ".png"));
	}
	setSprites(leftAnimation);
	*/
}
