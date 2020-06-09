#include "sre/SDLRenderer.hpp"
#include "sre/SpriteAtlas.hpp"
#include <SDL_events.h>
#include <vector>
#include "Box2D/Dynamics/b2World.h"
#include "GameObject.hpp"
#include "PlayerCamera.hpp"
#include "ThrowableComponent.hpp"
#include "Box2DDebugDraw.hpp"
#include "SDL_mixer.h"
#include "RayCastCallback.hpp"
#include "WorldObjectTypes.hpp"
#include "GameState.hpp"
#include "LevelProgression.hpp"

class PhysicsComponent;

struct WorldObjectTemplate {
	glm::vec2 pos;
	WorldObjectTypes type;
};

class ZombieGame : public b2ContactListener {
public:
	ZombieGame();

	std::shared_ptr<GameObject> createGameObject(bool isTile = false);
	static const glm::vec2 windowSize;

	void BeginContact(b2Contact* contact) override;

	void EndContact(b2Contact* contact) override;

	static ZombieGame* instance;

	void setGameState(GameState newState);

	WorldObjectTypes getWorldObjectTypeByName(std::string name);

	void incrementScore(int addition);

	GameState getGameState();

	void deleteGameObject(GameObject* gameObject);

	static glm::vec2 TranslateIsoToWorld(glm::vec2 point);
	static glm::vec2 TranslateWorldToIso(glm::vec2 point);

	std::shared_ptr<GameObject> PlaceTile(std::string tileName, glm::vec2 point);
	std::shared_ptr<GameObject> PlaceObject(std::string objectName, glm::vec2 point, bool isTile = false);

	std::shared_ptr<sre::SpriteAtlas> spriteAtlas;
	std::shared_ptr<sre::SpriteAtlas> spritePlayerWalkAtlas;
	std::shared_ptr<sre::SpriteAtlas> spriteGameAtlas;
	std::shared_ptr<sre::SpriteAtlas> spriteZombies;

	bool isOutsideMap(glm::vec2 pos);


	// Map related variables
	int spawnCount;
	float spawnRate;
	std::string mapName;

	int getEnemyCount();
	void decrementEnemyCount();

	/// Sound effects

	// Sound: General
	Mix_Chunk* scratch;
	Mix_Music* backgroundMusic;

	// Sound: Gravity Gun
	Mix_Chunk* gGShoot;
	Mix_Chunk* gGPickUp;

	// Sound: Player
	Mix_Chunk* playerSprint;
	Mix_Chunk* playerPain1;
	Mix_Chunk* playerPain2;
	Mix_Chunk* playerPain3;
	Mix_Chunk* warning;
	Mix_Chunk* flatline;
	Mix_Chunk* smallmedkit1;

	// Sound: Throwables
	Mix_Chunk* sawbladeSlice;
	Mix_Chunk* sawbladeSlice2;
	Mix_Chunk* sawbladeStick;
	Mix_Chunk* barrelImpact;
	Mix_Chunk* barrelImpact2;
	Mix_Chunk* barrelImpact3;

	// Sound: Zombies
	Mix_Chunk* zombieSpawnSound;
	Mix_Chunk* zombiePain1;
	Mix_Chunk* zombiePain2;
	Mix_Chunk* zombiePain3;
	Mix_Chunk* zombiePain4;
	Mix_Chunk* zombiePain5;
	Mix_Chunk* zombieSprint1;
	Mix_Chunk* zombieSprint2;
	Mix_Chunk* zombieSprint3;
	Mix_Chunk* zombieSprint4;
	Mix_Chunk* zombieSprint5;
	Mix_Chunk* zombieSprint6;
	Mix_Chunk* zombieDie1;
	Mix_Chunk* zombieDie2;
	Mix_Chunk* zombieDie3;
	Mix_Chunk* zombieAlert1;
	Mix_Chunk* zombieAlert2;
	Mix_Chunk* zombieAlert3;
	Mix_Chunk* zombieClawStrike1;
	Mix_Chunk* zombieClawStrike2;
	Mix_Chunk* zombieClawStrike3;

	// Helpers
	std::shared_ptr<ThrowableComponent> RayCast(b2Vec2 origin, b2Vec2 direction, float distance);

	// Spawnability
	std::shared_ptr<GameObject> createWorldObject(glm::vec2 pos, WorldObjectTypes type);

	// Stuff
	std::shared_ptr<GameObject> playerGameObject;
	const float physicsScale = 100;
	std::shared_ptr<GameObject> playerGo;

	glm::vec2 getMapDimensions();
	long getUid();
	int getScore();
private:
	bool* soundOn = new bool(true);
	int volume = 15;
	LevelProgression levelProgression;
	int enemyCount;
	long uidCounter = 0;
	std::map<std::string, WorldObjectTypes> typesMap;
	sre::SDLRenderer r;
	int score = 0;
	void init();
	void loadLevel(std::string levelPath);

	void initSounds();
	void initPhysics();

	glm::vec2 mapDimensions;

	std::vector<std::shared_ptr<GameObject>> sceneTiles;
	std::vector<std::shared_ptr<GameObject>> sceneObjects;

	void initWorldObjectTypes();

	void update(float time);
	void updateGameLogic();
	void nextLevel();
	void render();
	void renderGUI();
	void onKey(SDL_Event& event);
	void onMouseClick(SDL_Event& me);

	void handleContact(b2Contact* contact, bool begin);

	void deleteGameObjects();
	std::vector<GameObject*> toDelete;
	std::shared_ptr<PlayerCamera> camera;

	std::shared_ptr<GameObject> createWorldWallNorth();
	std::shared_ptr<GameObject> createWorldWallSouth();
	std::shared_ptr<GameObject> createWorldWallWest();
	std::shared_ptr<GameObject> createWorldWallEast();

	std::shared_ptr<GameObject> createObjectEnemy(glm::vec2 pos);
	std::shared_ptr<GameObject> createObjectEnemySpawner(glm::vec2 pos);
	std::shared_ptr<GameObject> createObjectPlayer(glm::vec2 pos);
	std::shared_ptr<GameObject> createObjectBarrel(glm::vec2 pos);
	std::shared_ptr<GameObject> createObjectMedkit(glm::vec2 pos);
	std::shared_ptr<GameObject> createObjectSawblade(glm::vec2 pos);
	std::shared_ptr<GameObject> createObjectBlackhole(glm::vec2 pos);
	std::shared_ptr<GameObject> createObjectRandomTree(glm::vec2 pos);

	std::vector<WorldObjectTemplate> toCreate;

	void updatePhysics();
	b2World* world = nullptr;


	void registerPhysicsComponent(PhysicsComponent* r);
	void deregisterPhysicsComponent(PhysicsComponent* r);
	std::map<b2Fixture*, PhysicsComponent*> physicsComponentLookup;
	Box2DDebugDraw debugDraw;
	bool doDebugDraw = false;
	GameState gameState = GameState::Running;
	friend class PhysicsComponent;
};