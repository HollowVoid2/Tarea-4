#ifndef MAZEGENERATOR_HPP
#define MAZEGENERATOR_HPP

#include <vector>
#include "BSPNode.hpp"

class MazeGenerator {
private:
    int width, height;
    int minRoomSize;
    int maxRoomSize;
    int maxDepth;
    int roomCounter;
    BSPNode* root;
    
    Room* metaRoom;
    bool metaRoomPlaced;
    bool metaRoomProtected;
    
    BSPNode* splitNode(BSPNode* node, int depth);
    void createRoom(BSPNode* node);
    Room* getRandomRoomFromNode(BSPNode* node);
    void connectSiblings(BSPNode* node);
    void connectAllRooms();
    bool areRoomsConnected(int id1, int id2);
    void connectTwoRooms(Room* r1, Room* r2);
    void createHorizontalCorridor(int x1, int x2, int y, int thickness = 1);
    void createVerticalCorridor(int y1, int y2, int x, int thickness = 1);
    void carveRooms();
    void markMetaRoom();
    void protectMetaRoom();
    
public:
    MazeGenerator(int _width, int _height);
    ~MazeGenerator();
    
    void generate();
    void clearMap();
    void regenerate();
    
    const std::vector<Room*>& getRooms() const { return rooms; }
    Room* getMetaRoom() const { return metaRoom; }
    bool hasMetaRoom() const { return metaRoomPlaced; }
    bool isMetaProtected() const { return metaRoomProtected; }
    
    std::vector<std::vector<char>> grid;
    std::vector<Room*> rooms;
    std::vector<std::pair<int, int>> connections;
};

#endif