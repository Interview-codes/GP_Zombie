#include "LevelProgression.hpp"
#include <iostream>

LevelProgression::LevelProgression() {
}

std::string LevelProgression::nextLevel() {
	int nextIndex = currentLevelIndex + 1;

	if (nextIndex > (getLevelCount() - 1)) return getCurrentLevel();

	currentLevelIndex++;
	return getCurrentLevel();
}
std::string LevelProgression::previousLevel() {
	int nextIndex = currentLevelIndex - 1;

	if (nextIndex < 0) return getCurrentLevel();

	currentLevelIndex--;
	return getCurrentLevel();
}

std::string LevelProgression::getFullLevelString(std::string level) {
	return "levels/" + level + ".json";
}

void LevelProgression::reset() {
	currentLevelIndex = 0;
}

int LevelProgression::getLevelCount() {
	return levels.size();
}
std::string LevelProgression::getCurrentLevel() {
	return levels.at(currentLevelIndex);
}
int LevelProgression::getCurrentLevelIndex() {
	return currentLevelIndex;
}