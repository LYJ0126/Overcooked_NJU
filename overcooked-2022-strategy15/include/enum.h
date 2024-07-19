#ifndef ENUM
#define ENUM
#include <cassert>
#include <string>

enum class ContainerKind {
    None,
    Pan,
    Pot,
    Plate,
    DirtyPlates,
};

enum class TileKind {
    None,
    Void,
    Floor,
    Wall,
    Table,
    IngredientBox,
    Trashbin,
    ChoppingStation,
    ServiceWindow,
    Stove,
    PlateReturn,
    Sink,
    PlateRack,
};

enum class State_person {
    None,
    Running,
    Interacting,
    Picking,
    Waiting,
    Thinking,
};

enum class Orient {
    None,
    Entity,
    Place,
    Organizer,
};

enum class Inter_direct {
    None,
    Left,
    Up,
    Right,
    Down,
};

enum {
    None,
    Granted,
    Restricted_player1,
    Restricted_player0,
    Denied_player1,
    Denied_player0,
    Restricted_wall,
    Denied_wall,
};


inline TileKind getTileKind(char kindChar) {
    switch (kindChar) {
    case '_':
        return TileKind::Void;
    case '.':
        return TileKind::Floor;
    case '*':
        return TileKind::Table;
    case 't':
        return TileKind::Trashbin;
    case 'c':
        return TileKind::ChoppingStation;
    case '$':
        return TileKind::ServiceWindow;
    case 's':
        return TileKind::Stove;
    case 'p':
        return TileKind::PlateReturn;
    case 'k':
        return TileKind::Sink;
    case 'r':
        return TileKind::PlateRack;
    default:
        assert(0);
    }
}

inline char getAbbrev(TileKind kind) {
    switch (kind) {
    case TileKind::IngredientBox:
        return 'i';
    case TileKind::Trashbin:
        return 't';
    case TileKind::ChoppingStation:
        return 'c';
    case TileKind::ServiceWindow:
        return '$';
    case TileKind::Stove:
        return 's';
    case TileKind::PlateReturn:
        return 'p';
    case TileKind::Sink:
        return 'k';
    case TileKind::PlateRack:
        return 'r';
    case TileKind::Floor:
        return '.';
    case TileKind::Void:
        return '_';
    default:
        assert(0);
    }
}

#endif