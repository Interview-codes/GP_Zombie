#include "SpriteAnimationComponent.hpp"
#include "GameObject.hpp"
#include <memory>


SpriteAnimationComponent::SpriteAnimationComponent(GameObject *gameObject) : Component(gameObject) {}

void SpriteAnimationComponent::update(float deltaTime) {
    auto spriteComponent = gameObject->getComponent<SpriteComponent>();

    assert(spriteComponent != nullptr);
    time += deltaTime;
    if (animationTime != 0 && time > animationTime){
        time = fmod(time, animationTime);
        spriteIndex = loop ? (spriteIndex + 1) % sprites.size() : glm::clamp(spriteIndex + 1, 0, (int)sprites.size() - 1);
        spriteComponent->setSprite(sprites[spriteIndex]);
    }
}

void SpriteAnimationComponent::setLoop(bool loop) {
	this->loop = loop;
}

void SpriteAnimationComponent::resetSpriteAnimation() {
	auto spriteComponent = gameObject->getComponent<SpriteComponent>();
	time = 0;
	spriteIndex = 0;
	spriteComponent->setSprite(sprites[0]);
}

void SpriteAnimationComponent::setSprites(std::vector<sre::Sprite> sprites) {
    this->sprites = sprites;
}

bool SpriteAnimationComponent::hasSprites() {
	return !sprites.empty();
}

float SpriteAnimationComponent::getAnimationTime() const {
    return animationTime;
}

void SpriteAnimationComponent::setAnimationTime(float animationTime) {
    SpriteAnimationComponent::animationTime = animationTime;
}
