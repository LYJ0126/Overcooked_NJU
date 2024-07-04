#include <interact.h>

std::string get_interact_direction(Point dst){
    if((int)dst.x == 0) return "L";
    else if((int)dst.x == width - 1) return "R";
    else if((int)dst.y == 0) return "U";
    else if((int)dst.y == height - 1) return "D";
    assert(0);
}

std::pair<int, std::string> Interact_with_object(Point dst){
    //找到实体
    std::string interact_direction = get_interact_direction(dst);
    for(int i = 0; i < entityCount; ++i){
        if(dst.x == Entity[i].x && dst.y == Entity[i].y){
            if(Entity[i].currentFrame == Entity[i].totalFrame - 1) return {1, "Interact " + interact_direction};//实体已经完成交互
            else return {0, "Interact " + interact_direction};
        }
    }
    return {0, "Interact " + interact_direction};
}