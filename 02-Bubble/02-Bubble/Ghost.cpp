#include "Ghost.h"

#define FLY_STEP 2
#define HITBOX_X 20
#define HITBOX_Y 21

enum GhostAnims {
    MOVE_LEFT,MOVE_RIGHT
};

Ghost::Ghost(bool left) {
    goLeft = left;
}

void Ghost::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) {
    spritesheet.loadFromFile("images/ghost.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(1.0f / 4.0f, 1.0f / 2.0f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(2);

    sprite->setAnimationSpeed(MOVE_LEFT, 6);
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.0f / 2.0f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(1.0f / 4.0f, 0.0f / 2.0f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f / 4.0f, 0.0f / 2.0f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(3.0f / 4.0f, 0.0f / 2.0f));

    sprite->setAnimationSpeed(MOVE_RIGHT, 6);
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0f / 4.0f, 1.0f / 2.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(1.0f / 4.0f, 1.0f / 2.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(2.0f / 4.0f, 1.0f / 2.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f / 4.0f, 1.0f / 2.0f));

    if (goLeft) sprite->changeAnimation(MOVE_LEFT);
    else sprite->changeAnimation(MOVE_RIGHT);
    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Ghost::update(int deltaTime) {
    sprite->update(deltaTime);

    if (goUp) {
        if (!map->collisionMoveDown(posEnemy, glm::ivec2(HITBOX_X, HITBOX_Y+1), &posEnemy.y, false)) {
            posEnemy.y += 1;
        }
        else goUp = !goUp;
    }
    else {
        if (!map->collisionMoveUp(posEnemy, glm::ivec2(HITBOX_X, 1), false)) {
            posEnemy.y -= 1;
        }
        else goUp = !goUp;
    }

    if (goLeft) {
        if (!map->collisionMoveLeft(posEnemy, glm::ivec2(HITBOX_X, HITBOX_Y), false, true)) {
            if (sprite->animation() != MOVE_LEFT) sprite->changeAnimation(MOVE_LEFT);
            posEnemy.x -= 1;
        }
        else goLeft = !goLeft;
    }
    else {
        if (!map->collisionMoveRight(posEnemy, glm::ivec2(HITBOX_X + 1, HITBOX_Y), false, true)) {
            if (sprite->animation() != MOVE_RIGHT) sprite->changeAnimation(MOVE_RIGHT);
            posEnemy.x += 1;
        }
        else goLeft = !goLeft;
    }

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}


glm::ivec2 Ghost::getBoundingBoxMax() {
    return posEnemy + glm::ivec2(HITBOX_X, HITBOX_Y);
}
glm::ivec2 Ghost::getBoundingBoxMin() {
    return posEnemy;
}
