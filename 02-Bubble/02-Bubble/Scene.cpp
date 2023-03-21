#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 32

#define INIT_PLAYER_X_TILES 4
#define INIT_PLAYER_Y_TILES 24

#define INIT_ENEMY_X_TILES 22
#define INIT_ENEMY_Y_TILES 24

#define INIT_COIN_X_TILES 12
#define INIT_COIN_Y_TILES 24

#define INIT_KEY_X_TILES 4
#define INIT_KEY_Y_TILES 24

#define INIT_DOOR_X_TILES 20
#define INIT_DOOR_Y_TILES 2


Scene::Scene()
{
	map = NULL;
	player = NULL;
	enemy = NULL;
	floorSprite = NULL;
	coin = NULL;
	key = NULL;
	door = NULL;
	keyTaken = false;
	openDoor = false;
	coinTaken = false;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
	if (enemy != NULL)
		delete enemy;
	if (floorSprite != NULL) {
		floorSprite->free();
		delete floorSprite;
	}
	if (coin != NULL) delete coin;
	if (door != NULL) delete door;
}


void Scene::init()
{
	initShaders();
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	enemy = new Ghost();
	enemy->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	enemy->setPosition(glm::vec2(INIT_ENEMY_X_TILES * map->getTileSize(), INIT_ENEMY_Y_TILES * map->getTileSize()));
	enemy->setTileMap(map);
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	
	backgroundTexture.loadFromFile("images/background1.png", TEXTURE_PIXEL_FORMAT_RGBA);
	background = Sprite::createSprite(glm::vec2(16*36, 16*28), glm::vec2(1, 1), &backgroundTexture, &texProgram);
	background->setPosition(glm::vec2(SCREEN_X, SCREEN_Y));

	spritesheet.loadFromFile("images/tileset.png", TEXTURE_PIXEL_FORMAT_RGBA);
	floorSprite = Sprite::createSprite(glm::vec2(16, 16), glm::vec2(0.1, 0.1), &spritesheet, &texProgram);

	floorSprite->setNumberAnimations(1);
	floorSprite->setAnimationSpeed(0, 8);
	floorSprite->addKeyframe(0, glm::vec2(0.2f, 0.f));
	floorSprite->changeAnimation(0);
	
	coin = new Coin();
	coin->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	coin->setPosition(glm::vec2(INIT_COIN_X_TILES * map->getTileSize(), INIT_COIN_Y_TILES * map->getTileSize()));
	coin->setTileMap(map);

	key = new Key();
	key->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	key->setTileMap(map);

	door = new Door();
	door->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	door->setPosition(glm::vec2(INIT_DOOR_X_TILES * map->getTileSize(), INIT_DOOR_Y_TILES * map->getTileSize()));
	door->setTileMap(map);

	vector<pair<int, int>> lavaMap = map->getLavaMap();
	for (auto& coord : lavaMap) {
		Lava* l = new Lava();
		l->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		l->setPosition(glm::vec2(coord.first * map->getTileSize(), coord.second * map->getTileSize()));
		l->setTileMap(map);
		lavas.push_back(l);
	}

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;

	keyTaken = false;
	openDoor = false;
	coinTaken = false;

}

bool Scene::collisionPlayerEnemy(Player* player, Enemy* enemy) {

	glm::ivec2 boundingBoxMaxPlayer = player->getBoundingBoxMax();
	glm::ivec2 boundingBoxMinPlayer = player->getBoundingBoxMin();
	glm::ivec2 boundingBoxMaxEnemy = enemy->getBoundingBoxMax();
	glm::ivec2 boundingBoxMinEnemy = enemy->getBoundingBoxMin();

	
	if (boundingBoxMinPlayer.x < boundingBoxMaxEnemy.x && boundingBoxMinEnemy.x < boundingBoxMaxPlayer.x && boundingBoxMinPlayer.y < boundingBoxMaxEnemy.y && boundingBoxMinEnemy.y < boundingBoxMaxPlayer.y) return true;
	return false;
}

bool Scene::collisionPlayerItem(Player* player, Item* item) {
	glm::ivec2 boundingBoxMaxPlayer = player->getBoundingBoxMax();
	glm::ivec2 boundingBoxMinPlayer = player->getBoundingBoxMin();
	glm::ivec2 boundingBoxMaxItem = item->getBoundingBoxMax();
	glm::ivec2 boundingBoxMinItem = item->getBoundingBoxMin();


	if (boundingBoxMinPlayer.x < boundingBoxMaxItem.x && boundingBoxMinItem.x < boundingBoxMaxPlayer.x && boundingBoxMinPlayer.y < boundingBoxMaxItem.y && boundingBoxMinItem.y < boundingBoxMaxPlayer.y) return true;
	return false;
}

void Scene::update(int deltaTime)
{

	currentTime += deltaTime;

	if (Game::instance().getKey('r')) reset();

	if (collisionPlayerEnemy(player, enemy)) {
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
		if (player->getLives() == 1) {
			reset();
		}
		else player->setLives(player->getLives() - 1);
	}

	for (unsigned int i = 0; i < lavas.size(); ++i) {
		if (collisionPlayerEnemy(player, lavas[i])) {
			player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
			if (player->getLives() == 1) {
				reset();
			}
			else player->setLives(player->getLives() - 1);
		}
	}

	if (collisionPlayerItem(player, coin)) {
		coinTaken = true;
	}

	if (collisionPlayerItem(player, key)) {
		keyTaken = true;
		door->openedDoor();
	}

	if (keyTaken && collisionPlayerItem(player, door)) {
		reset();
	}

	player->update(deltaTime);
	enemy->update(deltaTime);
	coin->update(deltaTime);
	door->update(deltaTime);

	for (auto& l : lavas) {
		l->update(deltaTime);
	}
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	background->render();

	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	map->render();

	for (auto& l : lavas) {
		l->render();
	}

	door->render();

	if(!coinTaken) coin->render();

	
	auto& floor = map->getFloor();
	
	int changedFloors = floor.size();

	for (auto it = floor.begin(); it != floor.end() && changedFloors > 0; ++it) {
		if (it->second) {
			floorSprite->setPosition(glm::vec2(it->first.first * map->getTileSize() + SCREEN_X, it->first.second * map->getTileSize() + SCREEN_Y));
			floorSprite->render();
			--changedFloors;
		}
	}

	if (changedFloors == 0 && !keyTaken) {
		key->setPosition(glm::vec2(INIT_KEY_X_TILES * map->getTileSize(), INIT_KEY_Y_TILES * map->getTileSize()));
		key->render();
	}

	enemy->render();
	player->render();

}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

void Scene::reset() {
	init();
}



