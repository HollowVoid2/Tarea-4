#include "MazeGenerator.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

MazeGenerator::MazeGenerator(int _width, int _height)
    : width(_width), height(_height), minRoomSize(6), maxRoomSize(15),
      maxDepth(8), roomCounter(0), root(nullptr), metaRoom(nullptr),
      metaRoomPlaced(false), metaRoomProtected(false) {
    grid.resize(height, std::vector<char>(width, '#'));
    srand(time(nullptr));
}

MazeGenerator::~MazeGenerator() {
    clearMap();
}

void MazeGenerator::generate() {
    clearMap();
    connections.clear();
    metaRoom = nullptr;
    metaRoomPlaced = false;
    metaRoomProtected = false;
    
    if (grid.empty() || grid[0].empty()) {
        grid.resize(height, std::vector<char>(width, '#'));
    }
    
    if (width < minRoomSize * 4 || height < minRoomSize * 4) {
        std::cerr << "Error: El mapa es demasiado pequeño" << std::endl;
        return;
    }
    
    root = new BSPNode(1, 1, width - 2, height - 2);
    splitNode(root, 0);
    carveRooms();
    
    markMetaRoom();
    protectMetaRoom();
    connectAllRooms();
    
    std::cout << "Habitaciones: " << rooms.size()
              << " | Conexiones: " << connections.size()
              << " | Meta: " << (metaRoom ? "Si" : "No")
              << " | Protegida: " << (metaRoomProtected ? "Si" : "No") << std::endl;
}

void MazeGenerator::regenerate() {
    std::cout << "Regenerando mapa..." << std::endl;
    generate();
}

BSPNode* MazeGenerator::splitNode(BSPNode* node, int depth) {
    if (depth >= maxDepth || node->w < minRoomSize * 3 || node->h < minRoomSize * 3) {
        node->isLeaf = true;
        createRoom(node);
        return node;
    }
    
    bool splitH = (rand() % 2 == 0);
    
    if (splitH && node->h >= minRoomSize * 3) {
        int splitPos = node->y + node->h / 2 + (rand() % 3 - 1) * 2;
        if (splitPos > node->y + minRoomSize && splitPos < node->y + node->h - minRoomSize) {
            node->left = new BSPNode(node->x, node->y, node->w, splitPos - node->y);
            node->right = new BSPNode(node->x, splitPos, node->w, node->y + node->h - splitPos);
            splitNode(node->left, depth + 1);
            splitNode(node->right, depth + 1);
            return node;
        }
    }
    
    if (!splitH && node->w >= minRoomSize * 3) {
        int splitPos = node->x + node->w / 2 + (rand() % 3 - 1) * 2;
        if (splitPos > node->x + minRoomSize && splitPos < node->x + node->w - minRoomSize) {
            node->left = new BSPNode(node->x, node->y, splitPos - node->x, node->h);
            node->right = new BSPNode(splitPos, node->y, node->x + node->w - splitPos, node->h);
            splitNode(node->left, depth + 1);
            splitNode(node->right, depth + 1);
            return node;
        }
    }
    
    node->isLeaf = true;
    createRoom(node);
    return node;
}

void MazeGenerator::createRoom(BSPNode* node) {
    if (node->w < minRoomSize + 3 || node->h < minRoomSize + 3) {
        return;
    }
    
    int maxRoomW = std::min(node->w - 3, maxRoomSize + 3);
    int maxRoomH = std::min(node->h - 3, maxRoomSize + 3);
    
    if (maxRoomW < minRoomSize || maxRoomH < minRoomSize) {
        return;
    }
    
    int roomW = minRoomSize + (rand() % (maxRoomW - minRoomSize + 1));
    int roomH = minRoomSize + (rand() % (maxRoomH - minRoomSize + 1));
    
    if (rand() % 3 == 0) {
        roomW = maxRoomW;
        roomH = maxRoomH;
    }
    
    roomW = std::max(minRoomSize, std::min(roomW, maxRoomW));
    roomH = std::max(minRoomSize, std::min(roomH, maxRoomH));
    
    int maxX = node->x + node->w - roomW - 1;
    int maxY = node->y + node->h - roomH - 1;
    
    if (maxX <= node->x + 1 || maxY <= node->y + 1) {
        return;
    }
    
    int roomX = node->x + 1 + (rand() % (maxX - node->x));
    int roomY = node->y + 1 + (rand() % (maxY - node->y));
    
    Room* newRoom = new Room(roomX, roomY, roomW, roomH, roomCounter++, ROOM_NORMAL);
    rooms.push_back(newRoom);
    node->room = newRoom;
}

Room* MazeGenerator::getRandomRoomFromNode(BSPNode* node) {
    if (!node) return nullptr;
    if (node->isLeaf && node->room) return node->room;
    
    if (rand() % 2 == 0) {
        Room* r = getRandomRoomFromNode(node->left);
        return r ? r : getRandomRoomFromNode(node->right);
    } else {
        Room* r = getRandomRoomFromNode(node->right);
        return r ? r : getRandomRoomFromNode(node->left);
    }
}

void MazeGenerator::connectSiblings(BSPNode* node) {
    if (!node || node->isLeaf) return;
    
    connectSiblings(node->left);
    connectSiblings(node->right);
    
    if (node->left && node->right) {
        Room* r1 = getRandomRoomFromNode(node->left);
        Room* r2 = getRandomRoomFromNode(node->right);
        
        if (r1 && r2) {
            bool r1IsMeta = (r1 == metaRoom && metaRoomProtected);
            bool r2IsMeta = (r2 == metaRoom && metaRoomProtected);
            
            if (r1IsMeta || r2IsMeta) {
                int metaConnections = 0;
                for (auto& conn : connections) {
                    if (conn.first == metaRoom->id || conn.second == metaRoom->id) {
                        metaConnections++;
                    }
                }
                if (metaConnections >= 1) {
                    return;
                }
            }
            
            connectTwoRooms(r1, r2);
            connections.push_back({r1->id, r2->id});
        }
    }
}

void MazeGenerator::connectAllRooms() {
    if (rooms.size() < 2) return;
    
    connectSiblings(root);
    
    int extraConnections = rooms.size() * 0.2;
    for (int i = 0; i < extraConnections; i++) {
        int r1 = rand() % rooms.size();
        int r2 = rand() % rooms.size();
        if (r1 != r2 && !areRoomsConnected(r1, r2)) {
            
            Room* room1 = rooms[r1];
            Room* room2 = rooms[r2];
            
            bool r1IsMeta = (room1 == metaRoom && metaRoomProtected);
            bool r2IsMeta = (room2 == metaRoom && metaRoomProtected);
            
            if (r1IsMeta || r2IsMeta) {
                int metaConnections = 0;
                for (auto& conn : connections) {
                    if (conn.first == metaRoom->id || conn.second == metaRoom->id) {
                        metaConnections++;
                    }
                }
                if (metaConnections >= 1) {
                    continue;
                }
            }
            
            connectTwoRooms(room1, room2);
            connections.push_back({r1, r2});
        }
    }
}

bool MazeGenerator::areRoomsConnected(int id1, int id2) {
    for (auto& conn : connections) {
        if ((conn.first == id1 && conn.second == id2) ||
            (conn.first == id2 && conn.second == id1)) {
            return true;
        }
    }
    return false;
}

void MazeGenerator::connectTwoRooms(Room* r1, Room* r2) {
    if (!r1 || !r2) return;
    
    int cx1 = r1->x + r1->w / 2;
    int cy1 = r1->y + r1->h / 2;
    int cx2 = r2->x + r2->w / 2;
    int cy2 = r2->y + r2->h / 2;
    
    int corridorWidth = 1 + (rand() % 2);
    
    if (rand() % 2 == 0) {
        createHorizontalCorridor(cx1, cx2, cy1, corridorWidth);
        createVerticalCorridor(cy1, cy2, cx2, corridorWidth);
    } else {
        createVerticalCorridor(cy1, cy2, cx1, corridorWidth);
        createHorizontalCorridor(cx1, cx2, cy2, corridorWidth);
    }
}

void MazeGenerator::createHorizontalCorridor(int x1, int x2, int y, int thickness) {
    int start = std::min(x1, x2);
    int end = std::max(x1, x2);
    for (int dy = 0; dy < thickness; dy++) {
        for (int x = start; x <= end; x++) {
            int cy = y + dy - thickness / 2;
            if (cy > 0 && cy < height && x > 0 && x < width) {
                grid[cy][x] = ' ';
            }
        }
    }
}

void MazeGenerator::createVerticalCorridor(int y1, int y2, int x, int thickness) {
    int start = std::min(y1, y2);
    int end = std::max(y1, y2);
    for (int dx = 0; dx < thickness; dx++) {
        for (int y = start; y <= end; y++) {
            int cx = x + dx - thickness / 2;
            if (y > 0 && y < height && cx > 0 && cx < width) {
                grid[y][cx] = ' ';
            }
        }
    }
}

void MazeGenerator::carveRooms() {
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

void MazeGenerator::markMetaRoom() {
    if (rooms.size() < 2) return;
    
    int metaIndex = rooms.size() - 1;
    metaRoom = rooms[metaIndex];
    metaRoom->type = ROOM_META;
    metaRoom->hasGoal = true;
    metaRoomPlaced = true;
    metaRoomProtected = false;
    
    std::cout << "Habitacion META: ID " << metaRoom->id
              << " en (" << metaRoom->x << ", " << metaRoom->y
              << ") tamano " << metaRoom->w << "x" << metaRoom->h << std::endl;
}

void MazeGenerator::protectMetaRoom() {
    if (!metaRoom) return;
    metaRoomProtected = true;
    std::cout << "Meta protegida: solo tendra 1 conexion" << std::endl;
}

void MazeGenerator::clearMap() {
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
    roomCounter = 0;
    metaRoom = nullptr;
    metaRoomPlaced = false;
    metaRoomProtected = false;
}