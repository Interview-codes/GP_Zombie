#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "WorldObjectTypes.hpp"


class WorldMap {
public:
	WorldMap(std::string path);
	glm::vec2 getSpawnPosition();
	int getSpawnCount();
	float getSpawnRate();
	int getTileAt(int x, int y);
	std::string getLevelName();
	std::string getTileNameFromInt(int num);
	glm::vec2 getMapDimensions();

	static bool hasCollision(int v);
	struct ObjectContainer {
	public:
		ObjectContainer(int _x, int _y, WorldObjectTypes _type);
		int x;
		int y;
		WorldObjectTypes type;
	};

	std::vector<ObjectContainer> getWorldObjects();
private:
	std::string levelName;
	glm::vec2 spawnPosition;
	int spawnCount;
	float spawnRate;
	std::vector<std::vector<int>> mapValues;
	std::vector<ObjectContainer> mapObjects;
	int mapWidth;
	int mapHeight;

};
