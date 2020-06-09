#include "GameObject.hpp"
#include <cassert>
#include <algorithm>
#include "Component.hpp"
#include "ZombieGame.hpp"

GameObject::~GameObject(){
    // remove reference to this in components
    for (auto& c : components){
        c->gameObject = nullptr;
    }
}

bool GameObject::removeComponent(std::shared_ptr<Component> component) {
    auto comp = std::find(components.begin(), components.end(),component);
    if (comp != components.end()){
        components.erase(comp);
    }
    return false;
}

const glm::vec2 &GameObject::getPosition() const {
    return position;
}

void GameObject::setPosition(const glm::vec2 &position) {
    GameObject::position = position;
}

float GameObject::getRotation() const {
    return rotation;
}

void GameObject::setRotation(float rotation) {
    GameObject::rotation = rotation;
	forward = glm::rotateZ(glm::vec3(1, 0, 0), glm::radians(rotation));;
}

int GameObject::getLayer() const {
	return layer;
}

void GameObject::setLayer(Layer layer) {
	GameObject::layer = (float) layer;
}

void GameObject::renderSprite(sre::SpriteBatch::SpriteBatchBuilder &spriteBatchBuilder) {
	for (auto& comp : components){
        comp->renderSprite(spriteBatchBuilder);		
    }
}

void GameObject::update(float deltaTime) {
    for (auto& comp : components){
        comp->update(deltaTime);
    }
}

long GameObject::getUid() {
	return uid;
}

void GameObject::setUid(long val) {
	uid = val;
}

const std::vector<std::shared_ptr<Component>> &GameObject::getComponents() {
    return components;
}
