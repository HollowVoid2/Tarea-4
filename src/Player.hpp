#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>

class Player {
private:
    float x, y;
    float speed;
    bool keys[4];
    
public:
    Player();
    
    void update(float deltaTime, const std::vector<std::vector<char>>& grid, int width, int height);
    void setKey(int key, bool pressed);
    
    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float _x, float _y) { x = _x; y = _y; }
    
    bool canMove(float newX, float newY, const std::vector<std::vector<char>>& grid, int width, int height) const;
};

#endif