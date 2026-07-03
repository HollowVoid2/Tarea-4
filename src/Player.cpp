#include "Player.hpp"
#include <algorithm>
#include <cmath>

Player::Player() : x(2.0f), y(2.0f), speed(4.0f) {
    for (int i = 0; i < 4; i++) keys[i] = false;
}

void Player::update(float deltaTime, const std::vector<std::vector<char>>& grid, int width, int height) {
    float newX = x;
    float newY = y;
    float moveDist = speed * deltaTime;
    
    if (keys[0]) newY -= moveDist;
    if (keys[1]) newX -= moveDist;
    if (keys[2]) newY += moveDist;
    if (keys[3]) newX += moveDist;
    
    if (canMove(newX, y, grid, width, height)) {
        x = newX;
    }
    if (canMove(x, newY, grid, width, height)) {
        y = newY;
    }
}

void Player::setKey(int key, bool pressed) {
    if (key >= 0 && key < 4) {
        keys[key] = pressed;
    }
}

bool Player::canMove(float newX, float newY, const std::vector<std::vector<char>>& grid, int width, int height) const {
    if (grid.empty() || grid[0].empty()) return false;
    
    int gridX = (int)(newX + 0.5f);
    int gridY = (int)(newY + 0.5f);
    
    if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height) {
        return false;
    }
    
    if (gridY >= (int)grid.size() || gridX >= (int)grid[gridY].size()) {
        return false;
    }
    
    return grid[gridY][gridX] != '#';
}