#include <iostream>
#include "ZombieGame.hpp"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "sre/Texture.hpp"
#include "SpriteComponent.hpp"
#include "SpriteAnimationComponent.hpp"
#include "CharacterAnimationComponent.hpp"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "PhysicsComponent.hpp"
#include "PlayerController.hpp"
#include "GravityGunComponent.hpp"
#include "EnemyComponent.hpp"
#include "EnemySpawnerComponent.hpp"
#include "BarrelComponent.hpp"
#include "SawbladeComponent.hpp"
#include "ThrowableComponent.hpp"
#include "MedkitComponent.hpp"
#include "WorldMap.hpp"
#include <map>
#include "Tile.hpp"
#include <time.h>


using namespace std;
using namespace sre;

const glm::vec2 ZombieGame::windowSize(800, 400);

ZombieGame* ZombieGame::instance = nullptr;

ZombieGame::ZombieGame()
	:debugDraw(physicsScale)
{
	srand(time(0)); // set rand() seed based on current time
	instance = this;

	r.setWindowSize(windowSize);
	r.init()
		.withSdlInitFlags(SDL_INIT_EVERYTHING)
		.withSdlWindowFlags(SDL_WINDOW_OPENGL);

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 vec = { windowSize.x, windowSize.y };
	io.DisplaySize = vec;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls


	std::string levelName = LevelProgression::getFullLevelString(levelProgression.getCurrentLevel());
	init();
	loadLevel(levelName);

	// setup callback functions
	r.keyEvent = [&](SDL_Event& e) {
		onKey(e);
	};
	r.mouseEvent = [&](SDL_Event& me) {
		onMouseClick(me);
	};
	r.frameUpdate = [&](float deltaTime) {
		update(deltaTime);
		deleteGameObjects(); // Delete gameobjects that were queued
	};
	r.frameRender = [&]() {
		render();
	};
	// start game loop
	r.startEventLoop();
}

void ZombieGame::initWorldObjectTypes() {
	typesMap["player"] = WorldObjectTypes::Player;
	typesMap["enemy"] = WorldObjectTypes::Enemy;
	typesMap["medkit"] = WorldObjectTypes::Medkit;
	typesMap["sawblade"] = WorldObjectTypes::Sawblade;
	typesMap["barrel"] = WorldObjectTypes::Barrel;
	typesMap["enemyspawner"] = WorldObjectTypes::EnemySpawner;
	typesMap["randomtree"] = WorldObjectTypes::RandomTree;
}

WorldObjectTypes ZombieGame::getWorldObjectTypeByName(std::string name) {
	std::map<std::string, WorldObjectTypes>::const_iterator pos = typesMap.find(name);

	if (pos == typesMap.end()) {
		assert(false);
	}
	else {
		WorldObjectTypes value = pos->second;
		return value;
	}
}

void ZombieGame::init() {
	initSounds();
	initWorldObjectTypes();

	Mix_PlayMusic(backgroundMusic, -1);

	/// sprite atlas
	spriteAtlas = SpriteAtlas::create("spritesheets/bird.json", "spritesheets/bird.png");
	spriteGameAtlas = SpriteAtlas::create("spritesheets/zombiegame.json", "spritesheets/zombiegame.png");
	spritePlayerWalkAtlas = SpriteAtlas::create("spritesheets/playerWalk.json", "spritesheets/playerWalk.png");
	spriteZombies = SpriteAtlas::create("spritesheets/zombies.json", "spritesheets/zombies.png");
}

void ZombieGame::loadLevel(std::string levelPath) {
	if (world != nullptr) { // deregister call backlistener to avoid getting callbacks when recreating the world
		world->SetContactListener(nullptr);
	}
	camera.reset();
	Mix_PauseMusic();
	sceneObjects.clear();
	sceneTiles.clear();
	uidCounter = 0;
	camera.reset();
	physicsComponentLookup.clear();
	initPhysics();

	gameState = GameState::Running;

	/// initialize camera
	auto camObj = createGameObject();
	camObj->name = "Camera";
	camera = camObj->addComponent<PlayerCamera>();
	camObj->setPosition(windowSize * 0.5f);

	/// start of scene
	WorldMap map = WorldMap(levelPath);
	glm::vec2 spawn_pos = TranslateIsoToWorld(map.getSpawnPosition());
	spawnCount = map.getSpawnCount();
	spawnRate = map.getSpawnRate();
	mapDimensions = map.getMapDimensions();
	mapName = map.getLevelName();

	std::cout << "Level loaded: " << map.getLevelName() << std::endl;

	/// Load map
	glm::vec2 mapDimensions = map.getMapDimensions();
	for (int row = 0; row < mapDimensions.x; row++) {
		for (int col = 0; col < mapDimensions.y; col++) {
			int tileNum = map.getTileAt(row, col);
			std::string tileName = map.getTileNameFromInt(tileNum);
			auto v = PlaceTile(tileName, glm::vec2(row, col));

			if (WorldMap::hasCollision(tileNum)) {
				auto physComp = v->addComponent<PhysicsComponent>();
				glm::vec2 pos = (v->getPosition() + glm::vec2(0, 10)) / physicsScale; // Maybe offset / size per tile
				physComp->initCircle(b2_staticBody, 23.f / physicsScale, pos, 1);
			}
		}
	}

	/// Create map walls
	auto wallnorth = createWorldWallNorth();
	auto wallsouth = createWorldWallSouth();
	auto wallwest = createWorldWallWest();
	auto walleast = createWorldWallEast();

	/// Load player
	glm::vec2 playerSpawnPosition = TranslateIsoToWorld(map.getSpawnPosition());
	playerGameObject = createWorldObject(playerSpawnPosition, WorldObjectTypes::Player);


	/// Load objects
	int spawnersCount = 0;
	std::vector<WorldMap::ObjectContainer> mapObjects = map.getWorldObjects();
	for (int i = 0; i < mapObjects.size(); i++) {
		glm::vec2 posIso(mapObjects[i].x, mapObjects[i].y);
		glm::vec2 posWorld = TranslateIsoToWorld(posIso);
		if (mapObjects[i].type == WorldObjectTypes::EnemySpawner) spawnersCount++;
		createWorldObject(posWorld, mapObjects[i].type);
	}
	
	// lazy fix for maps with no spawners
	enemyCount = (spawnersCount > 0) ? spawnersCount * map.getSpawnCount() : 99999;

	Mix_ResumeMusic();

	/// Debug stuff 

	/*
	for (int i = 0; i < 100; i++) {
		createWorldObject(TranslateIsoToWorld(glm::vec2(5, 5)), WorldObjectTypes::Enemy);
	}*/
}

int ZombieGame::getEnemyCount() {
	return enemyCount;
}

void ZombieGame::decrementEnemyCount() {
	enemyCount--;
	// std::cout << "Zombie killed. " << enemyCount << " remaining!" << std::endl;
}

void ZombieGame::initSounds() {
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Volume(-1, volume);
	Mix_VolumeMusic(volume);
	scratch = Mix_LoadWAV("audio/scratch.wav");

	gGPickUp = Mix_LoadWAV("audio/physcannon_pickup.wav");
	gGShoot = Mix_LoadWAV("audio/physcannon_launch.wav");
	backgroundMusic = Mix_LoadMUS("audio/BackgroundMusic.wav");

	playerPain1 = Mix_LoadWAV("audio/pl_pain5.wav");
	playerPain2 = Mix_LoadWAV("audio/pl_pain6.wav");
	playerPain3 = Mix_LoadWAV("audio/pl_pain7.wav");
	playerSprint = Mix_LoadWAV("audio/suit_sprint.wav");
	warning = Mix_LoadWAV("audio/warning.wav");
	flatline = Mix_LoadWAV("audio/flatline.wav");
	smallmedkit1 = Mix_LoadWAV("audio/smallmedkit1.wav");

	sawbladeSlice = Mix_LoadWAV("audio/flesh_squishy_impact_hard1.wav");
	sawbladeSlice2 = Mix_LoadWAV("audio/ZombieSlice.wav");
	sawbladeStick = Mix_LoadWAV("audio/sawblade_stick3.wav");
	barrelImpact = Mix_LoadWAV("audio/metal_barrel_impact_hard2.wav");
	barrelImpact2 = Mix_LoadWAV("audio/metal_barrel_impact_soft1.wav");
	barrelImpact3 = Mix_LoadWAV("audio/metal_barrel_impact_soft4.wav");

	zombieSpawnSound = Mix_LoadWAV("audio/combat_scan5.wav");
	zombiePain1 = Mix_LoadWAV("audio/zombie_pain1.wav");
	zombiePain2 = Mix_LoadWAV("audio/zombie_pain2.wav");
	zombiePain3 = Mix_LoadWAV("audio/zombie_pain3.wav");
	zombiePain4 = Mix_LoadWAV("audio/zombie_pain4.wav");
	zombiePain5 = Mix_LoadWAV("audio/zombie_pain5.wav");
	zombieSprint1 = Mix_LoadWAV("audio/growl1.wav");
	zombieSprint2 = Mix_LoadWAV("audio/growl2.wav");
	zombieSprint3 = Mix_LoadWAV("audio/growl3.wav");
	zombieSprint4 = Mix_LoadWAV("audio/growl4.wav");
	zombieSprint5 = Mix_LoadWAV("audio/growl5.wav");
	zombieSprint6 = Mix_LoadWAV("audio/growl6.wav");
	zombieDie1 = Mix_LoadWAV("audio/zombie_die1.wav");
	zombieDie2 = Mix_LoadWAV("audio/zombie_die2.wav");
	zombieDie3 = Mix_LoadWAV("audio/zombie_die3.wav");
	zombieAlert1 = Mix_LoadWAV("audio/zombie_alert1.wav");
	zombieAlert2 = Mix_LoadWAV("audio/zombie_alert2.wav");
	zombieAlert3 = Mix_LoadWAV("audio/zombie_alert3.wav");

	zombieClawStrike1 = Mix_LoadWAV("audio/claw_strike1.wav");
	zombieClawStrike2 = Mix_LoadWAV("audio/claw_strike2.wav");
	zombieClawStrike3 = Mix_LoadWAV("audio/claw_strike3.wav");
}

void ZombieGame::incrementScore(int addition) {
	score += addition;
	std::cout << "Score: " << score << std::endl;
}

std::shared_ptr<GameObject> ZombieGame::createWorldObject(glm::vec2 pos, WorldObjectTypes type) {
	switch (type) {
	case WorldObjectTypes::Player:
		return createObjectPlayer(pos);
	case WorldObjectTypes::Medkit:
		return createObjectMedkit(pos);
	case WorldObjectTypes::Sawblade:
		return createObjectSawblade(pos);
	case WorldObjectTypes::Enemy:
		return createObjectEnemy(pos);
	case WorldObjectTypes::EnemySpawner:
		return createObjectEnemySpawner(pos);
	case WorldObjectTypes::Barrel:
		return createObjectBarrel(pos);
	case WorldObjectTypes::RandomTree:
		return createObjectRandomTree(pos);
	default:
		std::cout << "Tried to parse type [int] " << (int)type << " and failed! Skipping.." << std::endl;
		assert(false);
	}
	return nullptr;
}

std::shared_ptr<GameObject> ZombieGame::createWorldWallWest() {
	glm::vec2 dimensions = getMapDimensions();
	float width = dimensions.x * ((float)Tile::TILE_WIDTH) / 2;
	glm::vec2 pos = TranslateIsoToWorld(glm::vec2(-.75, (dimensions.y / 2)));

	auto wallObj = createGameObject();
	wallObj->name = "WALL WEST";
	wallObj->setPosition(pos);
	auto phys = wallObj->addComponent<PhysicsComponent>();
	phys->initBox(b2_staticBody, glm::vec2{ width, 10 } / physicsScale,
		wallObj->getPosition() / physicsScale, 4);
	phys->body->SetTransform(phys->body->GetPosition(), glm::radians(26.5f));

	return wallObj;
}

std::shared_ptr<GameObject> ZombieGame::createWorldWallEast() {
	glm::vec2 dimensions = getMapDimensions();
	float width = dimensions.x * ((float)Tile::TILE_WIDTH) / 2;
	glm::vec2 pos = TranslateIsoToWorld(glm::vec2(dimensions.x - .25f, (dimensions.y / 2)));

	auto wallObj = createGameObject();
	wallObj->name = "WALL EAST";
	wallObj->setPosition(pos);
	auto phys = wallObj->addComponent<PhysicsComponent>();
	phys->initBox(b2_staticBody, glm::vec2{ width, 10 } / physicsScale,
		wallObj->getPosition() / physicsScale, 4);
	phys->body->SetTransform(phys->body->GetPosition(), glm::radians(26.5f));

	return wallObj;
}

std::shared_ptr<GameObject> ZombieGame::createWorldWallNorth() {
	glm::vec2 dimensions = getMapDimensions();
	float width = dimensions.x * ((float)Tile::TILE_WIDTH) / 2;
	glm::vec2 pos = TranslateIsoToWorld(glm::vec2((dimensions.x / 2), -.75));

	auto wallObj = createGameObject();
	wallObj->name = "WALL NORTH";
	wallObj->setPosition(pos);
	auto phys = wallObj->addComponent<PhysicsComponent>();
	phys->initBox(b2_staticBody, glm::vec2{ width, 10 } / physicsScale,
		wallObj->getPosition() / physicsScale, 4);
	phys->body->SetTransform(phys->body->GetPosition(), glm::radians(-26.5f));

	return wallObj;
}

std::shared_ptr<GameObject> ZombieGame::createWorldWallSouth() {
	glm::vec2 dimensions = getMapDimensions();
	float width = dimensions.x * ((float)Tile::TILE_WIDTH) / 2;
	glm::vec2 pos = TranslateIsoToWorld(glm::vec2((dimensions.x / 2), dimensions.y - .25f));

	auto wallObj = createGameObject();
	wallObj->name = "WALL SOUTH";
	wallObj->setPosition(pos);
	auto phys = wallObj->addComponent<PhysicsComponent>();
	phys->initBox(b2_staticBody, glm::vec2{ width, 10 } / physicsScale,
		wallObj->getPosition() / physicsScale, 4);
	phys->body->SetTransform(phys->body->GetPosition(), glm::radians(-26.5f));

	return wallObj;
}

std::shared_ptr<GameObject> ZombieGame::createObjectBarrel(glm::vec2 pos) {
	float barrelHeight = 0.25;
	float barrelWidth = 0.12;
	auto barObj = createGameObject();
	barObj->name = "Barrel";
	barObj->setPosition(pos);
	barObj->setLayer(Layer::Med);
	auto barPhyscomp = barObj->addComponent<PhysicsComponent>();
	barPhyscomp->initBox(b2_dynamicBody, glm::vec2{ barrelWidth / 2, barrelHeight / 2 },
		pos / physicsScale, 4);
	auto barComp = barObj->addComponent<BarrelComponent>();
	auto barSpriteComp = barObj->addComponent<SpriteComponent>();
	auto barSpr = spriteGameAtlas->get("Barrel.png");
	barSpr.setScale({ 0.5,0.5 });
	barSpriteComp->setSprite(barSpr);

	return barObj;
}

std::shared_ptr<GameObject> ZombieGame::createObjectPlayer(glm::vec2 pos) {
	auto playerObj = createGameObject();
	playerObj->name = "Player";
	playerObj->setLayer(Layer::Med);
	camera->setFollowObject(playerObj);
	auto so = playerObj->addComponent<SpriteComponent>();
	auto sprite = spritePlayerWalkAtlas->get("player_walk_down1.png");
	playerObj->setPosition(pos);
	so->setSprite(sprite);
	auto phys = playerObj->addComponent<PhysicsComponent>();
	phys->initBox(
		b2_dynamicBody,
		{ 10 / physicsScale, 25 / physicsScale },
		{ (playerObj->getPosition().x / physicsScale), playerObj->getPosition().y / physicsScale },
		2
	);
	auto anim = playerObj->addComponent<CharacterAnimationComponent>();
	anim->setUpAnimation(CharacterAnimationComponent::Type::Player);
	anim->lookAtMouse = true;
	auto playerC = playerObj->addComponent<PlayerController>();
	auto gravityGunObj = createGameObject();
	gravityGunObj->name = "GravityGun";
	auto ggo = gravityGunObj->addComponent<GravityGunComponent>();
	ggo->setFollowObject(playerObj);
	auto ggPhysComp = gravityGunObj->addComponent<PhysicsComponent>();
	auto ggsc = gravityGunObj->addComponent<SpriteComponent>();
	auto ggSprite = spriteGameAtlas->get("gravitygun.png");
	ggSprite.setScale({ 0.8, 0.8 });
	ggsc->setSprite(ggSprite);
	playerC->setGravityGun(gravityGunObj);

	return playerObj;
}

std::shared_ptr<GameObject> ZombieGame::createObjectMedkit(glm::vec2 pos) {
	auto go = createGameObject();
	go->name = "Object - Medkit";
	auto spritecomp = go->addComponent<SpriteComponent>();
	auto medkitComp = go->addComponent<MedkitComponent>();
	auto sprite = spriteGameAtlas->get("Medkit.png");
	sprite.setScale({0.4f, 0.4f});
	go->setPosition(pos);
	spritecomp->setSprite(sprite);
	go->setLayer(Layer::TileTexture);
	auto phys = go->addComponent<PhysicsComponent>();
	phys->initCircle(b2_kinematicBody, 15 / physicsScale, pos / physicsScale, 1);
	phys->setSensor(true);
	return go;
}

std::shared_ptr<GameObject> ZombieGame::createObjectRandomTree(glm::vec2 pos) {
	std::string treeString;
	int value = rand() % 6;
	switch (value) {
	case 0:
		treeString = "tree_light1.png";
		break;
	case 1:
		treeString = "tree_light2.png";
		break;
	case 2:
		treeString = "tree_light3.png";
		break;
	case 3:
		treeString = "tree1.png";
		break;
	case 4:
		treeString = "tree2.png";
		break;
	case 5:
		treeString = "tree3.png";
		break;
	default:
		treeString = "tree1.png";
		break;
	}

	glm::vec2 offset = glm::vec2(0, 64);
	auto go = createGameObject();
	go->name = "Object - Random Tree";
	auto spritecomp = go->addComponent<SpriteComponent>();
	auto sprite = spriteGameAtlas->get(treeString);
	sprite.setScale({ 1,1 });
	go->setPosition(pos + offset);
	spritecomp->setSprite(sprite);
	go->setLayer(Layer::Above);
	auto phys = go->addComponent<PhysicsComponent>();
	phys->initBox(b2_staticBody, glm::vec2(6, 12) / physicsScale, pos / physicsScale, 1);
	//phys->initCircle(b2_staticBody, 15 / physicsScale, pos / physicsScale, 1);
	return go;
}

std::shared_ptr<GameObject> ZombieGame::createObjectSawblade(glm::vec2 pos) {
	auto go = createGameObject();
	go->name = "Object - Sawblade";
	auto spritecomp = go->addComponent<SpriteComponent>();
	auto sprite = spriteGameAtlas->get("sawblade.png"); // CHANGE
	sprite.setScale({ 0.4,0.4 });
	go->setPosition(pos);
	spritecomp->setSprite(sprite);
	go->setLayer(Layer::High);
	auto phys = go->addComponent<PhysicsComponent>();
	phys->initCircle(b2_dynamicBody, 9 / physicsScale, pos / physicsScale, 4);

	auto sawComp = go->addComponent<SawbladeComponent>();
	return go;
}

std::shared_ptr<GameObject> ZombieGame::createObjectEnemy(glm::vec2 pos) {
	auto enemyObj = createGameObject();
	auto enemyPhysComp = enemyObj->addComponent<PhysicsComponent>();
	enemyObj->setPosition(pos);
	enemyObj->setLayer(Layer::Med);
	enemyPhysComp->initBox(b2_dynamicBody, { 10 / physicsScale, 25 / physicsScale },
		{ (enemyObj->getPosition().x / physicsScale), (enemyObj->getPosition().y) / physicsScale },
		.5f);
	enemyObj->name = "Object - Enemy";
	auto enemySpriteComp = enemyObj->addComponent<SpriteComponent>();
	auto anim = enemyObj->addComponent<CharacterAnimationComponent>();
	switch (rand() % 4) {
	case(0):
		anim->setUpAnimation(CharacterAnimationComponent::Type::Dark_Female);
		break;
	case(1):
		anim->setUpAnimation(CharacterAnimationComponent::Type::Dark_Male);
		break;
	case(2):
		anim->setUpAnimation(CharacterAnimationComponent::Type::Light_Female);
		break;
	case(3):
		anim->setUpAnimation(CharacterAnimationComponent::Type::Light_Male);
		break;
	}

	auto enemyEnemyComponent = enemyObj->addComponent<EnemyComponent>();
	enemyEnemyComponent->setTarget(playerGameObject);

	return enemyObj;
}



std::shared_ptr<GameObject> ZombieGame::createObjectEnemySpawner(glm::vec2 pos) {
	auto enemySpawnerObj = createGameObject();
	enemySpawnerObj->setLayer(Layer::TileTexture);
	enemySpawnerObj->setPosition(pos);
	enemySpawnerObj->name = "Object - EnemySpawner";
	auto testsprite = enemySpawnerObj->addComponent<SpriteComponent>();

	Sprite v = spriteAtlas->get("tile078.png");
	v.setScale({ .5,.5 });

	testsprite->setSprite(v);
	enemySpawnerObj->addComponent<EnemySpawnerComponent>();

	return enemySpawnerObj;
}

/*
std::shared_ptr<GameObject> ZombieGame::createObjectBlackhole(glm::vec2 pos) {

	auto blackholeObj = createGameObject();
	auto blackholePhysComp = blackholeObj->addComponent<PhysicsComponent>();
	blackholePhysComp->initCircle(b2_dynamicBody, 10 / physicsScale,
		glm::vec2(pos.x / physicsScale, pos.y / physicsScale), 50);
	blackholeObj->name = "Object - Blackhole";
	blackholeObj->setPosition(TranslateIsoToWorld(pos));
	auto blackholeSpriteComp = blackholeObj->addComponent<SpriteComponent>();
	blackholeSpriteComp->setSprite(spriteAtlas->get("green_line.png"));
	auto blackholeComponent = blackholeObj->addComponent<BlackholeComponent>();

	blackholeComponent->setFollowObject(playerGameObject);
	return blackholeObj;
}
*/


glm::vec2 ZombieGame::TranslateIsoToWorld(glm::vec2 point) {
	auto x = (point.x - point.y) * (float)Tile::TILE_WIDTH_HALF;
	auto y = -(point.x + point.y) * (float)Tile::TILE_HEIGHT_HALF;

	return glm::vec2(x, y);
}

glm::vec2 ZombieGame::TranslateWorldToIso(glm::vec2 point) {
	auto x = (point.x / (float)Tile::TILE_WIDTH_HALF - point.y / (float)Tile::TILE_HEIGHT_HALF) / 2;
	auto y = -(point.y / (float)Tile::TILE_HEIGHT_HALF + (point.x / (float)Tile::TILE_WIDTH_HALF)) / 2;

	return glm::vec2(x, y);
}

std::shared_ptr<GameObject> ZombieGame::PlaceTile(string tileName, glm::vec2 point)
{
	return PlaceObject(tileName, point, true);
}

bool ZombieGame::isOutsideMap(glm::vec2 position) {
	glm::vec2 dimensions = ZombieGame::instance->getMapDimensions();
	glm::vec2 currentPosition = ZombieGame::TranslateWorldToIso(position);

	if (currentPosition.x > (dimensions.x - 1) || currentPosition.x < 1) return true;
	if (currentPosition.y > (dimensions.y - 1) || currentPosition.y < 1) return true;

	return false;
}

std::shared_ptr<GameObject> ZombieGame::PlaceObject(string objectName, glm::vec2 point, bool isTile)
{
	auto sprite = spriteAtlas->get(objectName);
	auto obj = createGameObject(isTile);
	obj->name = objectName;
	auto so = obj->addComponent<SpriteComponent>();

	obj->setPosition(TranslateIsoToWorld(point));
	so->setSprite(sprite);

	return obj;
}


void ZombieGame::update(float deltaTime) {
	if (gameState == GameState::Running) {
		updatePhysics();
		for (int i = 0; i < sceneObjects.size(); i++) {
			sceneObjects[i]->update(deltaTime);
		}
		
		updateGameLogic();
	}
}
int ZombieGame::getScore() {
	return score;
}

void ZombieGame::updateGameLogic() {
	if (enemyCount == 0) nextLevel();
}

void ZombieGame::nextLevel() {
	std::string levelName = LevelProgression::getFullLevelString(levelProgression.nextLevel());
	loadLevel(levelName);
}

bool sorter(std::shared_ptr<GameObject> i, std::shared_ptr<GameObject> j) { return ((i->getPosition().y / 100) < (j->getPosition().y / 100)); }

void ZombieGame::render() {
	auto rp = RenderPass::create()
		.withCamera(camera->getCamera())
		.build();

	auto pos = camera->getGameObject()->getPosition();
	auto spriteBatchBuilder = SpriteBatch::create();

	for (auto& go : sceneTiles) {
		go->renderSprite(spriteBatchBuilder);
	}

	for (auto& go : sceneObjects) {
		go->renderSprite(spriteBatchBuilder);
	}

	if (gameState == GameState::Ready) {
		auto sprite = spriteAtlas->get("get-ready.png");
		sprite.setPosition(pos);
		sprite.setOrderInBatch(INT_MAX);
		spriteBatchBuilder.addSprite(sprite);
	}
	else if (gameState == GameState::GameOver) {
		auto sprite = spriteAtlas->get("game-over.png");
		sprite.setPosition(pos);
		sprite.setOrderInBatch(INT_MAX);
		spriteBatchBuilder.addSprite(sprite);
	}
	auto sb = spriteBatchBuilder.build();
	rp.draw(sb);

	if (doDebugDraw) {
		world->DrawDebugData();
		rp.drawLines(debugDraw.getLines());
		debugDraw.clear();
	}

	renderGUI();
}

void ZombieGame::renderGUI() {
	// Game info gui
	auto player = playerGameObject->getComponent<PlayerController>();
	if (!player) return;

	ImGui::SetNextWindowPos(ImVec2(Renderer::instance->getWindowSize().x / 2 - 100, .0f), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_Always);
	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);


	ImGui::Text(mapName.c_str());
	ImGui::Text("Score: %i", score);
	ImGui::Text("Enemies: %i", getEnemyCount());
	ImGui::Text("Suit Energy: %i", player->getSuitEnergy());
	ImGui::Text("HP: %i", player->getHealth());
	
	ImGui::End();
	
	// Game options gui
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(170, 150), ImGuiSetCond_Always);
	ImGui::Begin("Game Options", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	if (ImGui::Button("Restart")) {
		score = 0;
		levelProgression.reset();
		std::string levelName = LevelProgression::getFullLevelString(levelProgression.getCurrentLevel());
		gameState = GameState::Ready;
		loadLevel(levelName);
	}
	if (ImGui::Button("Pause")) {
		setGameState(GameState::Ready);
	}
	if (ImGui::Button("Start")) {
		if (gameState == GameState::GameOver) {
			score = 0;
			levelProgression.reset();
			std::string levelName = LevelProgression::getFullLevelString(levelProgression.getCurrentLevel());
			gameState = GameState::Ready;
			loadLevel(levelName);
		}
		else if (gameState == GameState::Ready) {
			gameState = GameState::Running;
		}
	}
	
	ImGui::DragInt("Volume", &volume, 1.0f, 0, 127);
	ImGui::Checkbox("Sound", soundOn);
	if (!(*soundOn)) {
		Mix_Volume(-1, 0);
		Mix_VolumeMusic(0);
	}
	else {
		Mix_Volume(-1, volume);
		Mix_VolumeMusic(volume);
	}
	ImGui::End();

}

void ZombieGame::onMouseClick(SDL_Event& event) {
	for (auto& gameObject : sceneObjects) {
		for (auto& c : gameObject->getComponents()) {
			bool consumed = c->onMouse(event);
			if (consumed) {
				return;
			}
		}
	}
	
}

void ZombieGame::onKey(SDL_Event& event) {
	for (auto& gameObject : sceneObjects) {
		for (auto& c : gameObject->getComponents()) {
			bool consumed = c->onKey(event);
			if (consumed) {
				return;
			}
		}
	}
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_e:
			// press 'e' for debug
			doDebugDraw = !doDebugDraw;
			if (doDebugDraw) {
				world->SetDebugDraw(&debugDraw);
				playerGameObject->getComponent<PlayerController>()->isDebug = true;
			}
			else {
				world->SetDebugDraw(nullptr);
				playerGameObject->getComponent<PlayerController>()->isDebug = false;
			}
			break;
		case SDLK_n:
			if (!doDebugDraw) break;
			std::cout << "#######" << std::endl;
			std::cout << "Scene Objects: " << sceneObjects.size() << std::endl;
			std::cout << "Scene Tiles: " << sceneTiles.size() << std::endl;;
			std::cout << "#######" << std::endl;
			break;
		case SDLK_LEFT: {
			if (!doDebugDraw) break;
			std::string levelString = LevelProgression::getFullLevelString(levelProgression.previousLevel());
			loadLevel(levelString);
			break;
		}
		case SDLK_RIGHT: {
			if (!doDebugDraw) break;
			std::string levelString = LevelProgression::getFullLevelString(levelProgression.nextLevel());
			loadLevel(levelString);
			break;
		}
		}
	}
}

std::shared_ptr<GameObject> ZombieGame::createGameObject(bool isTile) {
	auto obj = make_shared<GameObject>(GameObject());
	obj->setUid(getUid());
	if (isTile) sceneTiles.push_back(obj);
	else sceneObjects.push_back(obj);
	return obj;
}

std::shared_ptr<ThrowableComponent> ZombieGame::RayCast(b2Vec2 origin, b2Vec2 direction, float distance) {
	RaysCastCallback callback;

	b2Vec2 tOrigin = { origin.x / physicsScale, origin.y / physicsScale };
	b2Vec2 endPoint = tOrigin + distance * direction;

	world->RayCast(&callback, tOrigin, { endPoint.x, endPoint.y });

	if (callback.m_fixture) {
		for each (auto obj in sceneObjects) {
			auto pComp = obj->getComponent<PhysicsComponent>();
			if (pComp) {
				if (pComp->fixture == callback.m_fixture) {
					return obj->getComponent<ThrowableComponent>();
				}
			}
		}
	}
	return nullptr;
}

void ZombieGame::deleteGameObject(GameObject* gameObject) {
	toDelete.push_back(gameObject);
}

void ZombieGame::deleteGameObjects() {
	for (int i = 0; i < toDelete.size(); i++) {
		for (int j = 0; j < sceneObjects.size(); j++) {
			if (sceneObjects[j].get() == toDelete[i]) {
				sceneObjects.erase(sceneObjects.begin() + j);
				break;
			}
		}
	}
	toDelete.clear();
}

void ZombieGame::updatePhysics() {
	const float32 timeStep = 1.0f / 60.0f;
	const int positionIterations = 2;
	const int velocityIterations = 6;
	world->Step(timeStep, velocityIterations, positionIterations);

	for (auto phys : physicsComponentLookup) {
		if (phys.second->rbType == b2_staticBody) continue;
		auto position = phys.second->body->GetPosition();
		auto gameObject = phys.second->getGameObject();

		gameObject->setPosition(glm::vec2(position.x * physicsScale,
			position.y * physicsScale));
		phys.second->body->SetTransform(b2Vec2(position.x, position.y), gameObject->getRotation());

	}
}

void ZombieGame::initPhysics() {
	delete world; // R.I.P. world
	world = new b2World(b2Vec2(0, 0));
	world->SetContactListener(this);

	if (doDebugDraw) {
		world->SetDebugDraw(&debugDraw);
	}
}

void ZombieGame::BeginContact(b2Contact* contact) {
	b2ContactListener::BeginContact(contact);
	handleContact(contact, true);
}

void ZombieGame::EndContact(b2Contact* contact) {
	b2ContactListener::EndContact(contact);
	handleContact(contact, false);
}

void ZombieGame::deregisterPhysicsComponent(PhysicsComponent* r) {
	auto iter = physicsComponentLookup.find(r->fixture);
	if (iter != physicsComponentLookup.end()) {
		physicsComponentLookup.erase(iter);
	}
	else {
		assert(false); // cannot find physics object
	}
}

void ZombieGame::registerPhysicsComponent(PhysicsComponent* r) {
	physicsComponentLookup[r->fixture] = r;
}


void ZombieGame::handleContact(b2Contact* contact, bool begin) {
	auto fixA = contact->GetFixtureA();
	auto fixB = contact->GetFixtureB();
	auto physA = physicsComponentLookup.find(fixA);
	auto physB = physicsComponentLookup.find(fixB);
	if (physA != physicsComponentLookup.end() && physB != physicsComponentLookup.end()) {
		auto& aComponents = physA->second->getGameObject()->getComponents();
		auto& bComponents = physB->second->getGameObject()->getComponents();
		for (auto& c : aComponents) {
			if (begin) {
				c->onCollisionStart(physB->second);
			}
			else {
				c->onCollisionEnd(physB->second);
			}
		}
		for (auto& c : bComponents) {
			if (begin) {
				c->onCollisionStart(physA->second);
			}
			else {
				c->onCollisionEnd(physA->second);
			}
		}
	}
}

long ZombieGame::getUid() {
	uidCounter++;
	return uidCounter;
}

glm::vec2 ZombieGame::getMapDimensions() {
	return mapDimensions;
}

void ZombieGame::setGameState(GameState newState) {
	this->gameState = newState;
}

GameState ZombieGame::getGameState() {
	return this->gameState;
}
