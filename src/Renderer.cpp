#include "Renderer.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

Renderer::Renderer(int w, int h) : windowWidth(w), windowHeight(h) {}

void Renderer::drawMaze(const std::vector<std::vector<char>>& grid) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (grid.empty() || grid[0].empty()) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(windowWidth, 0);
        glVertex2f(windowWidth, windowHeight);
        glVertex2f(0, windowHeight);
        glEnd();
        return;
    }
    
    int mazeWidth = grid[0].size();
    int mazeHeight = grid.size();
    
    if (mazeWidth == 0 || mazeHeight == 0) return;
    
    float cellW = (float)windowWidth / mazeWidth;
    float cellH = (float)windowHeight / mazeHeight;
    
    for (int y = 0; y < mazeHeight; y++) {
        for (int x = 0; x < mazeWidth; x++) {
            float posX = x * cellW;
            float posY = windowHeight - (y + 1) * cellH;
            
            if (grid[y][x] == '#') {
                glColor3f(0.15f, 0.15f, 0.25f);
            } else {
                glColor3f(0.85f, 0.82f, 0.75f);
            }
            
            glBegin(GL_QUADS);
            glVertex2f(posX, posY);
            glVertex2f(posX + cellW, posY);
            glVertex2f(posX + cellW, posY + cellH);
            glVertex2f(posX, posY + cellH);
            glEnd();
        }
    }
}

void Renderer::drawPlayer(float x, float y, const std::vector<std::vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) return;
    
    int mazeWidth = grid[0].size();
    int mazeHeight = grid.size();
    
    if (mazeWidth == 0 || mazeHeight == 0) return;
    
    float cellW = (float)windowWidth / mazeWidth;
    float cellH = (float)windowHeight / mazeHeight;
    
    x = std::max(0.5f, std::min((float)mazeWidth - 0.5f, x));
    y = std::max(0.5f, std::min((float)mazeHeight - 0.5f, y));
    
    float posX = x * cellW;
    float posY = windowHeight - (y + 1) * cellH;
    float size = cellW * 0.6f;
    
    glColor3f(0.0f, 0.8f, 0.0f);
    int segments = 20;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(posX + cellW/2, posY + cellH/2);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float dx = cos(angle) * size/2;
        float dy = sin(angle) * size/2;
        glVertex2f(posX + cellW/2 + dx, posY + cellH/2 + dy);
    }
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float eyeSize = size * 0.2f;
    glBegin(GL_QUADS);
    glVertex2f(posX + cellW/2 + size*0.2f, posY + cellH/2 + size*0.1f);
    glVertex2f(posX + cellW/2 + size*0.2f + eyeSize, posY + cellH/2 + size*0.1f);
    glVertex2f(posX + cellW/2 + size*0.2f + eyeSize, posY + cellH/2 + size*0.1f + eyeSize);
    glVertex2f(posX + cellW/2 + size*0.2f, posY + cellH/2 + size*0.1f + eyeSize);
    glEnd();
}

void Renderer::drawMetaGoal(const Room* metaRoom, const std::vector<std::vector<char>>& grid) {
    if (!metaRoom || grid.empty() || grid[0].empty()) return;
    
    int mazeWidth = grid[0].size();
    int mazeHeight = grid.size();
    
    if (mazeWidth == 0 || mazeHeight == 0) return;
    
    float cellW = (float)windowWidth / mazeWidth;
    float cellH = (float)windowHeight / mazeHeight;
    
    float cx = metaRoom->getCenterX();
    float cy = metaRoom->getCenterY();
    
    float posX = cx * cellW;
    float posY = windowHeight - (cy + 1) * cellH;
    float size = cellW * 0.7f;
    
    float pulse = 0.8f + 0.2f * sin(glfwGetTime() * 2.0f);
    
    glColor3f(1.0f * pulse, 0.9f * pulse, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(posX + cellW/2 - size/2, posY + cellH/2 - size/2);
    glVertex2f(posX + cellW/2 + size/2, posY + cellH/2 - size/2);
    glVertex2f(posX + cellW/2 + size/2, posY + cellH/2 + size/2);
    glVertex2f(posX + cellW/2 - size/2, posY + cellH/2 + size/2);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 0.5f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(posX + cellW/2 - size/2, posY + cellH/2 - size/2);
    glVertex2f(posX + cellW/2 + size/2, posY + cellH/2 - size/2);
    glVertex2f(posX + cellW/2 + size/2, posY + cellH/2 + size/2);
    glVertex2f(posX + cellW/2 - size/2, posY + cellH/2 + size/2);
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    float textW = size * 0.5f;
    float textH = size * 0.15f;
    glBegin(GL_QUADS);
    glVertex2f(posX + cellW/2 - textW/2, posY + cellH/2 - textH/2);
    glVertex2f(posX + cellW/2 + textW/2, posY + cellH/2 - textH/2);
    glVertex2f(posX + cellW/2 + textW/2, posY + cellH/2 + textH/2);
    glVertex2f(posX + cellW/2 - textW/2, posY + cellH/2 + textH/2);
    glEnd();
}

void Renderer::drawRoomDebug(const Room* room) {
    if (!room) return;
    
    float cellW = (float)windowWidth / 80;
    float cellH = (float)windowHeight / 60;
    
    float posX = room->x * cellW;
    float posY = windowHeight - (room->y + room->h) * cellH;
    float w = room->w * cellW;
    float h = room->h * cellH;
    
    if (room->type == ROOM_META) {
        glColor3f(1.0f, 1.0f, 0.0f);
    } else {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(posX, posY);
    glVertex2f(posX + w, posY);
    glVertex2f(posX + w, posY + h);
    glVertex2f(posX, posY + h);
    glEnd();
}

bool Renderer::checkMetaCollision(float playerX, float playerY, const Room* metaRoom) {
    if (!metaRoom) return false;
    
    float cubeCX = metaRoom->getCenterX();
    float cubeCY = metaRoom->getCenterY();
    
    float cubeSize = 0.7f / 2.0f;
    float playerSize = 0.5f;
    
    float dx = std::abs(playerX - cubeCX);
    float dy = std::abs(playerY - cubeCY);
    
    return (dx < cubeSize + playerSize) && (dy < cubeSize + playerSize);
}