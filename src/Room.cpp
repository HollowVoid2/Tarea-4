#include "Room.hpp"

Room::Room(int _x, int _y, int _w, int _h, int _id, RoomType _type)
    : x(_x), y(_y), w(_w), h(_h), id(_id), type(_type), hasGoal(false) {}

float Room::getCenterX() const {
    return x + w / 2.0f;
}

float Room::getCenterY() const {
    return y + h / 2.0f;
}