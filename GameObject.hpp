#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "sre/SpriteBatch.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "SpriteAnimationComponent.hpp"
#include "Component.hpp"
#include "Layer.hpp"
#include <SDL_events.h>

// Forward declaration
class Component;

// GameObject are empty container objects, which contains Components
class GameObject {
public:

	~GameObject();

	template <class T>                                                  // Add component of a given type to a gameObject. example:
	std::shared_ptr<T> addComponent();                   // std::shared_ptr<SpriteComponent> spriteComponent = gameObject->addComponent<SpriteComponent>();

	template <class T>                                   //  Get component of a given type to a gameObject. If not found return empty shared_ptr (==nullptr). example:
	std::shared_ptr<T> getComponent();                   // std::shared_ptr<SpriteComponent> spriteComponent = gameObject->getComponent<SpriteComponent>();

	bool removeComponent(std::shared_ptr<Component> component);

	void renderSprite(sre::SpriteBatch::SpriteBatchBuilder& spriteBatchBuilder);
	void update(float deltaTime);

	const glm::vec2& getPosition() const;

	void setPosition(const glm::vec2& position);

	float getRotation() const;

	void setRotation(float rotation);

	int getLayer() const;

	void setLayer(Layer layer);

	const std::vector<std::shared_ptr<Component>>& getComponents();

	std::string name = "_";

	std::string tag = "";

	glm::vec2 forward = glm::vec2(1, 0);

	GameObject() = default; // moved to public

	long getUid();

	void setUid(long val);

private:
	std::vector<std::shared_ptr<Component>> components;
	glm::vec2 position;

	long uid;
	float layer = 0;
	float rotation;

	friend class ZombieGame;
};

// definition of the template member function addComponent
// Usage:
// GameObject* go = something;
// shared_ptr<SpriteComponent> sc = go->addComponent<SpriteComponent>();
template <class T>
inline std::shared_ptr<T> GameObject::addComponent() {
	auto obj = std::shared_ptr<T>(new T(this));
	components.push_back(obj);

	return obj;
}

// definition of the template member function addComponent
// Usage:
// GameObject* go = something;
// shared_ptr<SpriteComponent> sc = go->addComponent<SpriteComponent>();
template <class T>
inline std::shared_ptr<T> GameObject::getComponent() {
	for (auto c : components) {
		std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(c);
		if (res != nullptr) {
			return res;
		}
	}
	return std::shared_ptr<T>();
}
