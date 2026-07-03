#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "MazeGenerator.hpp"
#include "Player.hpp"
#include "Renderer.hpp"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const int MAZE_WIDTH = 80;
const int MAZE_HEIGHT = 60;

MazeGenerator* g_generator = nullptr;
Player* g_player = nullptr;
Renderer* g_renderer = nullptr;

void resetGame() {
    if (g_generator && g_player) {
        std::cout << "Regenerando mapa..." << std::endl;
        g_generator->regenerate();
        if (!g_generator->getRooms().empty()) {
            Room* firstRoom = g_generator->getRooms()[0];
            g_player->setPosition(firstRoom->getCenterX(), firstRoom->getCenterY());
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        resetGame();
    }
    
    if (g_player) {
        if (key == GLFW_KEY_W) g_player->setKey(0, action != GLFW_RELEASE);
        if (key == GLFW_KEY_A) g_player->setKey(1, action != GLFW_RELEASE);
        if (key == GLFW_KEY_S) g_player->setKey(2, action != GLFW_RELEASE);
        if (key == GLFW_KEY_D) g_player->setKey(3, action != GLFW_RELEASE);
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          "Mazmorra BSP - Habitacion META",
                                          nullptr, nullptr);
    
    if (!window) {
        std::cerr << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        return -1;
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    std::cout << "=== MAZMORRA BSP - HABITACION META ===" << std::endl;
    std::cout << "Controles:" << std::endl;
    std::cout << "  WASD - Moverse" << std::endl;
    std::cout << "  R    - Regenerar mapa" << std::endl;
    std::cout << "  ESC  - Salir" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Encuentra el cubo AMARILLO para ganar" << std::endl;
    
    MazeGenerator generator(MAZE_WIDTH, MAZE_HEIGHT);
    Player player;
    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    g_generator = &generator;
    g_player = &player;
    g_renderer = &renderer;
    
    generator.generate();
    if (!generator.getRooms().empty()) {
        Room* firstRoom = generator.getRooms()[0];
        player.setPosition(firstRoom->getCenterX(), firstRoom->getCenterY());
    }
    
    double lastTime = glfwGetTime();
    bool metaTriggered = false;
    
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;
        
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        
        player.update(deltaTime, generator.grid, MAZE_WIDTH, MAZE_HEIGHT);
        
        if (generator.hasMetaRoom() && !metaTriggered) {
            bool collidesWithCube = renderer.checkMetaCollision(
                player.getX(),
                player.getY(),
                generator.getMetaRoom()
            );
            
            if (collidesWithCube) {
                metaTriggered = true;
                std::cout << "Has tocado el cubo AMARILLO" << std::endl;
                resetGame();
                metaTriggered = false;
            }
        }
        
        renderer.drawMaze(generator.grid);
        renderer.drawMetaGoal(generator.getMetaRoom(), generator.grid);
        renderer.drawPlayer(player.getX(), player.getY(), generator.grid);
        
        char title[150];
        sprintf(title, "Mazmorra BSP - Habitaciones: %zu - Meta: %s - Protegida: %s",
                generator.getRooms().size(),
                generator.hasMetaRoom() ? "Si" : "No",
                generator.isMetaProtected() ? "Si" : "No");
        glfwSetWindowTitle(window, title);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    g_generator = nullptr;
    g_player = nullptr;
    g_renderer = nullptr;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}