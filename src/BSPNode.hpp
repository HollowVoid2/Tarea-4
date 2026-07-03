#ifndef BSPNODE_HPP
#define BSPNODE_HPP

#include "Room.hpp"

struct BSPNode {
    int x, y, w, h;
    BSPNode* left;
    BSPNode* right;
    Room* room;
    bool isLeaf;
    
    BSPNode(int _x, int _y, int _w, int _h);
    ~BSPNode();
    
    bool isRoomValid() const;
};

#endif