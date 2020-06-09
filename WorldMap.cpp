#include "WorldMap.hpp"
#include "ZombieGame.hpp"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>
#include <iostream>

using namespace std;

WorldMap::WorldMap(std::string path)
{
	ifstream ifs(path);
	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document d;
	d.ParseStream(isw);

	levelName = d["name"].GetString();
	spawnPosition = glm::vec2(d["properties"]["spawn"]["x"].GetFloat(), d["properties"]["spawn"]["y"].GetFloat());
	spawnCount = d["properties"]["spawncount"].GetInt();
	spawnRate = d["properties"]["spawnrate"].GetFloat();

	/// Tiles
	mapWidth = d["map"][0].GetArray().Size();
	mapHeight = d["map"].GetArray().Size();
	for (int row = 0; row < mapHeight; row++) {
		std::vector<int> currentRow;
		for (int col = 0; col < mapWidth; col++) {
			int cell = d["map"][col][row].GetInt();
			currentRow.push_back(cell);
		}
		mapValues.push_back(currentRow);
	}

	/// Objects
	int objectsSize = d["objects"].GetArray().Size();
	for (int i = 0; i < objectsSize; i++) {
		int xVal = d["objects"][i]["x"].GetInt();
		int yVal = d["objects"][i]["y"].GetInt();
		std::string typeString = d["objects"][i]["type"].GetString();

		WorldObjectTypes type = ZombieGame::instance->getWorldObjectTypeByName(typeString);
		auto obj = ObjectContainer(xVal, yVal, type);
		mapObjects.push_back(obj);
	}

	ifs.close();
}

std::string WorldMap::getLevelName() {
	return levelName;
}

std::vector<WorldMap::ObjectContainer> WorldMap::getWorldObjects() {
	return mapObjects;
}

WorldMap::ObjectContainer::ObjectContainer(int _x, int _y, WorldObjectTypes _type) {
	x = _x;
	y = _y;
	type = _type;
}

int WorldMap::getSpawnCount() {
	return spawnCount;
}

float WorldMap::getSpawnRate() {
	return spawnRate;
}
glm::vec2 WorldMap::getMapDimensions() {
	return glm::vec2(mapWidth, mapHeight);
}

int WorldMap::getTileAt(int x, int y)
{
	return mapValues[x][y];
}

/// Hacky solution. Converts "0" to "tile000.png"
std::string WorldMap::getTileNameFromInt(int num) {
	assert(num >= 0);

	std::string base = "tile0";
	if (num < 10) base += "0";
	std::string end = ".png";
	std::string numAsString = std::to_string(num);
	return base + numAsString + end;
}

glm::vec2 WorldMap::getSpawnPosition() {
	return spawnPosition;
}

bool WorldMap::hasCollision(int v) {
	switch (v)
	{
	case 20:
	case 22:
	case 23:
	case 24:
	case 44:
	case 46:
	case 47:
	case 68:
	case 70:
	case 71:
	case 92:
	case 94:
	case 95:
		return true;
	default:
		return false;
		break;
	}
}