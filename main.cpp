#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <memory>

// ============ FORWARD DECLARATIONS ============
class MazeGenerator;

// ============ ESTRUCTURAS ============
struct Room {
    int x, y, w, h;
    Room(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
};

struct BSPNode {
    int x, y, w, h;
    BSPNode* left;
    BSPNode* right;
    Room* room;
    bool isLeaf;
    
    BSPNode(int _x, int _y, int _w, int _h) 
        : x(_x), y(_y), w(_w), h(_h), left(nullptr), right(nullptr), room(nullptr), isLeaf(false) {}
    
    ~BSPNode() {
        delete left;
        delete right;
        room = nullptr;
    }
};

// ============ VARIABLES GLOBALES ============
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const int MAZE_WIDTH = 80;
const int MAZE_HEIGHT = 60;

std::vector<std::vector<char>> grid;
std::vector<Room*> rooms;

float playerX = 2.0f;
float playerY = 2.0f;
float playerSpeed = 4.0f;
bool keys[4] = {false, false, false, false};

MazeGenerator* g_generator = nullptr;

// ============ CLASE MAZEGENERATOR ============
class MazeGenerator {
private:
    int width, height;
    int minRoomSize;
    int maxRoomSize;
    int maxDepth;
    BSPNode* root;
    
    BSPNode* splitNode(BSPNode* node, int depth) {
        // Primero verificar si el nodo es demasiado pequeño para dividir
        if (depth >= maxDepth || node->w < minRoomSize * 2 || node->h < minRoomSize * 2) {
            node->isLeaf = true;
            createRoom(node);
            return node;
        }
        
        bool splitH = (rand() % 2 == 0);
        
        if (splitH && node->h >= minRoomSize * 2) {
            int splitPos = node->y + node->h / 2 + (rand() % 3 - 1) * 2;
            if (splitPos > node->y + minRoomSize && splitPos < node->y + node->h - minRoomSize) {
                node->left = new BSPNode(node->x, node->y, node->w, splitPos - node->y);
                node->right = new BSPNode(node->x, splitPos, node->w, node->y + node->h - splitPos);
                splitNode(node->left, depth + 1);
                splitNode(node->right, depth + 1);
                return node;
            }
        }
        
        if (!splitH && node->w >= minRoomSize * 2) {
            int splitPos = node->x + node->w / 2 + (rand() % 3 - 1) * 2;
            if (splitPos > node->x + minRoomSize && splitPos < node->x + node->w - minRoomSize) {
                node->left = new BSPNode(node->x, node->y, splitPos - node->x, node->h);
                node->right = new BSPNode(splitPos, node->y, node->x + node->w - splitPos, node->h);
                splitNode(node->left, depth + 1);
                splitNode(node->right, depth + 1);
                return node;
            }
        }
        
        // Si no se pudo dividir, es hoja
        node->isLeaf = true;
        createRoom(node);
        return node;
    }
    
    void createRoom(BSPNode* node) {
        // VALIDACIÓN: Asegurar que el nodo tiene espacio para una habitación
        if (node->w < minRoomSize + 2 || node->h < minRoomSize + 2) {
            // Nodo demasiado pequeño para una habitación
            return;
        }
        
        // Calcular tamaño máximo de habitación seguro
        int maxRoomW = std::min(node->w - 2, maxRoomSize);
        int maxRoomH = std::min(node->h - 2, maxRoomSize);
        
        // Asegurar que hay espacio para la habitación
        if (maxRoomW < minRoomSize || maxRoomH < minRoomSize) {
            return;
        }
        
        // Calcular tamaño de la habitación (con validación)
        int roomW = minRoomSize + (rand() % (maxRoomW - minRoomSize + 1));
        int roomH = minRoomSize + (rand() % (maxRoomH - minRoomSize + 1));
        
        // Asegurar que roomW y roomH son válidos
        roomW = std::max(minRoomSize, std::min(roomW, maxRoomW));
        roomH = std::max(minRoomSize, std::min(roomH, maxRoomH));
        
        // Calcular posición (con validación)
        int maxX = node->x + node->w - roomW - 1;
        int maxY = node->y + node->h - roomH - 1;
        
        if (maxX <= node->x + 1 || maxY <= node->y + 1) {
            return; // No hay espacio para la habitación
        }
        
        int roomX = node->x + 1 + (rand() % (maxX - node->x));
        int roomY = node->y + 1 + (rand() % (maxY - node->y));
        
        // Crear habitación
        Room* newRoom = new Room(roomX, roomY, roomW, roomH);
        rooms.push_back(newRoom);
        node->room = newRoom;
    }
    
    void connectRooms() {
        if (rooms.size() < 2) return;
        
        connectNodes(root);
        
        for (int i = 0; i < rooms.size() * 0.3; i++) {
            int r1 = rand() % rooms.size();
            int r2 = rand() % rooms.size();
            if (r1 != r2) {
                connectTwoRooms(rooms[r1], rooms[r2]);
            }
        }
    }
    
    void connectNodes(BSPNode* node) {
        if (!node) return;
        
        if (node->left && node->right) {
            if (node->left->room && node->right->room) {
                connectTwoRooms(node->left->room, node->right->room);
            }
        }
        
        connectNodes(node->left);
        connectNodes(node->right);
    }
    
    void connectTwoRooms(Room* r1, Room* r2) {
        if (!r1 || !r2) return;
        
        int cx1 = r1->x + r1->w / 2;
        int cy1 = r1->y + r1->h / 2;
        int cx2 = r2->x + r2->w / 2;
        int cy2 = r2->y + r2->h / 2;
        
        if (rand() % 2 == 0) {
            createHorizontalCorridor(cx1, cx2, cy1);
            createVerticalCorridor(cy1, cy2, cx2);
        } else {
            createVerticalCorridor(cy1, cy2, cx1);
            createHorizontalCorridor(cx1, cx2, cy2);
        }
    }
    
    void createHorizontalCorridor(int x1, int x2, int y) {
        int start = std::min(x1, x2);
        int end = std::max(x1, x2);
        for (int x = start; x <= end; x++) {
            if (y > 0 && y < height && x > 0 && x < width) {
                grid[y][x] = ' ';
            }
        }
    }
    
    void createVerticalCorridor(int y1, int y2, int x) {
        int start = std::min(y1, y2);
        int end = std::max(y1, y2);
        for (int y = start; y <= end; y++) {
            if (y > 0 && y < height && x > 0 && x < width) {
                grid[y][x] = ' ';
            }
        }
    }
    
    void carveRooms() {
        for (Room* room : rooms) {
            for (int y = room->y; y < room->y + room->h; y++) {
                for (int x = room->x; x < room->x + room->w; x++) {
                    if (y > 0 && y < height && x > 0 && x < width) {
                        grid[y][x] = ' ';
                    }
                }
            }
        }
    }
    
public:
    MazeGenerator(int _width, int _height) 
        : width(_width), height(_height), minRoomSize(4), maxRoomSize(10), maxDepth(10), root(nullptr) {
        grid.resize(height, std::vector<char>(width, '#'));
        srand(time(nullptr));
    }
    
    void generate() {
        clearMap();
        
        if (grid.empty() || grid[0].empty()) {
            grid.resize(height, std::vector<char>(width, '#'));
        }
        
        // Verificar que el espacio sea suficiente
        if (width < minRoomSize * 4 || height < minRoomSize * 4) {
            std::cerr << "Error: El mapa es demasiado pequeño para generar habitaciones" << std::endl;
            return;
        }
        
        root = new BSPNode(1, 1, width - 2, height - 2);
        splitNode(root, 0);
        carveRooms();
        connectRooms();
        
        if (!rooms.empty()) {
            playerX = rooms[0]->x + rooms[0]->w / 2.0f;
            playerY = rooms[0]->y + rooms[0]->h / 2.0f;
        } else {
            playerX = width / 2.0f;
            playerY = height / 2.0f;
            std::cerr << "Advertencia: No se generaron habitaciones" << std::endl;
        }
        
        std::cout << "Habitaciones generadas: " << rooms.size() << std::endl;
    }
    
    void clearMap() {
        delete root;
        root = nullptr;
        
        if (!grid.empty()) {
            for (int y = 0; y < height && y < (int)grid.size(); y++) {
                for (int x = 0; x < width && x < (int)grid[y].size(); x++) {
                    grid[y][x] = '#';
                }
            }
        }
        
        for (Room* room : rooms) {
            delete room;
        }
        rooms.clear();
    }
    
    ~MazeGenerator() {
        clearMap();
    }
};

// ============ FUNCIONES DE DIBUJO ============
void drawMaze(const std::vector<std::vector<char>>& maze) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (maze.empty() || maze[0].empty()) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(WINDOW_WIDTH, 0);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
        glVertex2f(0, WINDOW_HEIGHT);
        glEnd();
        return;
    }
    
    int mazeWidth = maze[0].size();
    int mazeHeight = maze.size();
    
    if (mazeWidth == 0 || mazeHeight == 0) return;
    
    float cellW = (float)WINDOW_WIDTH / mazeWidth;
    float cellH = (float)WINDOW_HEIGHT / mazeHeight;
    
    for (int y = 0; y < mazeHeight; y++) {
        for (int x = 0; x < mazeWidth; x++) {
            float posX = x * cellW;
            float posY = WINDOW_HEIGHT - (y + 1) * cellH;
            
            if (maze[y][x] == '#') {
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

void drawPlayer(float x, float y) {
    if (grid.empty() || grid[0].empty()) return;
    
    int mazeWidth = grid[0].size();
    int mazeHeight = grid.size();
    
    if (mazeWidth == 0 || mazeHeight == 0) return;
    
    float cellW = (float)WINDOW_WIDTH / mazeWidth;
    float cellH = (float)WINDOW_HEIGHT / mazeHeight;
    
    // Limitar posición del jugador al mapa
    x = std::max(0.5f, std::min((float)mazeWidth - 0.5f, x));
    y = std::max(0.5f, std::min((float)mazeHeight - 0.5f, y));
    
    float posX = x * cellW;
    float posY = WINDOW_HEIGHT - (y + 1) * cellH;
    float size = cellW * 0.6f;
    
    // Cuerpo del jugador
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
    
    // Ojos
    glColor3f(1.0f, 1.0f, 1.0f);
    float eyeSize = size * 0.2f;
    glBegin(GL_QUADS);
    glVertex2f(posX + cellW/2 + size*0.2f, posY + cellH/2 + size*0.1f);
    glVertex2f(posX + cellW/2 + size*0.2f + eyeSize, posY + cellH/2 + size*0.1f);
    glVertex2f(posX + cellW/2 + size*0.2f + eyeSize, posY + cellH/2 + size*0.1f + eyeSize);
    glVertex2f(posX + cellW/2 + size*0.2f, posY + cellH/2 + size*0.1f + eyeSize);
    glEnd();
}

// ============ MOVIMIENTO Y COLISIONES ============
bool canMove(float x, float y) {
    if (grid.empty() || grid[0].empty()) return false;
    
    int gridX = (int)(x + 0.5f);
    int gridY = (int)(y + 0.5f);
    
    if (gridX < 0 || gridX >= MAZE_WIDTH || gridY < 0 || gridY >= MAZE_HEIGHT) {
        return false;
    }
    
    if (gridY >= (int)grid.size() || gridX >= (int)grid[gridY].size()) {
        return false;
    }
    
    return grid[gridY][gridX] != '#';
}

void updatePlayer(float deltaTime) {
    float newX = playerX;
    float newY = playerY;
    float moveDist = playerSpeed * deltaTime;
    
    if (keys[0]) newY += moveDist;
    if (keys[1]) newX -= moveDist;
    if (keys[2]) newY -= moveDist;
    if (keys[3]) newX += moveDist;
    
    if (canMove(newX, playerY)) {
        playerX = newX;
    }
    if (canMove(playerX, newY)) {
        playerY = newY;
    }
}

// ============ CALLBACKS ============
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        if (g_generator) {
            std::cout << "Regenerando mapa..." << std::endl;
            g_generator->generate();
        }
    }
    
    if (key == GLFW_KEY_W) keys[0] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_A) keys[1] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_S) keys[2] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_D) keys[3] = (action != GLFW_RELEASE);
}

// ============ MAIN ============
int main() {
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                          "Mazmorra BSP - Corregido", 
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
    
    std::cout << "=== MAZMORRA BSP - VERSIÓN CORREGIDA ===" << std::endl;
    std::cout << "Controles:" << std::endl;
    std::cout << "  WASD - Moverse" << std::endl;
    std::cout << "  R    - Regenerar mapa" << std::endl;
    std::cout << "  ESC  - Salir" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    MazeGenerator generator(MAZE_WIDTH, MAZE_HEIGHT);
    g_generator = &generator;
    
    generator.generate();
    
    double lastTime = glfwGetTime();
    
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;
        
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        
        updatePlayer(deltaTime);
        drawMaze(grid);
        drawPlayer(playerX, playerY);
        
        char title[100];
        sprintf(title, "Mazmorra BSP - Habitaciones: %zu - Jugador (%.1f, %.1f)", 
                rooms.size(), playerX, playerY);
        glfwSetWindowTitle(window, title);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    g_generator = nullptr;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}