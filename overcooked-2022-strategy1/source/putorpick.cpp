#include <putorpick.h>

std::string get_put_or_pick_direction(Point dst)
{
    if((int)dst.x == 0) return "L";
    else if((int)dst.x == width - 1) return "R";
    else if((int)dst.y == 0) return "U";
    else if((int)dst.y == height - 1) return "D";
    assert(0);
}

std::pair<int, std::string> Put_object(Point dst, int player_id)
{
    std::string put_or_pick_direction = get_put_or_pick_direction(dst);
    return {1, "PutOrPick " + put_or_pick_direction};
}

std::pair<int, std::string> Pick_object(Point dst, std::string object, int player_id)
{
    std::string put_or_pick_direction = get_put_or_pick_direction(dst);
    if(object == "Pan" || object == "Pot"){//需要等待食物烹饪完成再拿
        //找到实体
        for(int i = 0; i < entityCount; ++i){
            if(Entity[i].entity[0] == object){
                if(Entity[i].currentFrame >= Entity[i].totalFrame) return {1, "PutOrPick " + put_or_pick_direction};//实体已经完成交互
                else return {0, "Move "};//等待
            }
        }
    }
    return {1, "PutOrPick " + put_or_pick_direction};
}