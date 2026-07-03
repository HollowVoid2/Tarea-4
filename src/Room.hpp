#ifndef ROOM_HPP
#define ROOM_HPP

enum RoomType {
    ROOM_NORMAL,
    ROOM_META
};

struct Room {
    int x, y, w, h;
    int id;
    RoomType type;
    bool hasGoal;
    
    Room(int _x, int _y, int _w, int _h, int _id = -1, RoomType _type = ROOM_NORMAL);
    
    float getCenterX() const;
    float getCenterY() const;
};

#endif