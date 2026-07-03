#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include "Room.hpp"

class Renderer {
private:
    int windowWidth, windowHeight;
    
public:
    Renderer(int w, int h);
    
    void drawMaze(const std::vector<std::vector<char>>& grid);
    void drawPlayer(float x, float y, const std::vector<std::vector<char>>& grid);
    void drawMetaGoal(const Room* metaRoom, const std::vector<std::vector<char>>& grid);
    void drawRoomDebug(const Room* room);
    
    bool checkMetaCollision(float playerX, float playerY, const Room* metaRoom);
};

#endif