#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Skeleton.h"
#include "Sprite.h"
#include "Texture.h"
#include "StaticSprite.h"
#include "Coin.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();
	
	bool collisionPlayerEnemy(Player* player, Enemy* enemy);
	bool collisionPlayerItem(Player* player, Item* item);

private:
	void initShaders();

private:
	TileMap *map;
	Player *player;
	Skeleton* enemy;
	Texture spritesheet;
	Texture backgroundTexture;
	Sprite* background;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	Sprite* floorSprite;
	Coin* coin;
	bool openDoor;

};


#endif // _SCENE_INCLUDE

