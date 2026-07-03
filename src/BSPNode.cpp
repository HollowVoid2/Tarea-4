#include "BSPNode.hpp"

BSPNode::BSPNode(int _x, int _y, int _w, int _h)
    : x(_x), y(_y), w(_w), h(_h), left(nullptr), right(nullptr), room(nullptr), isLeaf(false) {}

BSPNode::~BSPNode() {
    delete left;
    delete right;
    room = nullptr;
}

bool BSPNode::isRoomValid() const {
    return isLeaf && room != nullptr;
}