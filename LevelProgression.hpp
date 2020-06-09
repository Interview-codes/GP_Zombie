#pragma once

#include <string>
#include <vector>

class LevelProgression {                                           // Abstract class that adds behavior to a GameObject
public:
	LevelProgression();
	std::string nextLevel();
	std::string previousLevel();
	int getLevelCount();
	std::string getCurrentLevel();
	int getCurrentLevelIndex();
	void reset();

	static std::string getFullLevelString(std::string level);
protected:
	std::vector<std::string> levels = { "level0", "level1", "level2" };
	int currentLevelIndex = 0;
};
