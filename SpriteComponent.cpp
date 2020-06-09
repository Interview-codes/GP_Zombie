#include "SpriteComponent.hpp"
#include "GameObject.hpp"
#include <iostream>

SpriteComponent::SpriteComponent(GameObject *gameObject) : Component(gameObject) {}

void SpriteComponent::renderSprite(sre::SpriteBatch::SpriteBatchBuilder &spriteBatchBuilder) {
    sprite.setPosition(gameObject->getPosition());
	sprite.setOrderInBatch(gameObject->getLayer());
    sprite.setRotation(gameObject->getRotation());
    spriteBatchBuilder.addSprite(sprite);
}

void SpriteComponent::setSprite(const sre::Sprite &sprite) {
    this->sprite = sprite;
}

sre::Sprite SpriteComponent::getSprite() {
    return sprite;
}
