#include <iostream>
#include <cmath>
#include <move.h>

/*bool aboout_to_arrive(double dst_x, double dst_y, double player_x, double player_y, double x_velocity, double y_velocity){//判断是否即将到达下一个格子
    //double nxt_x = src.x + (double)nxt[player_direction][0];
    //double nxt_y = src.y + (double)nxt[player_direction][1];
    double distance = sqrt((dst_x - player_x) * (dst_x - player_x) + (dst_y - player_y) * (dst_y - player_y));
    if((x_velocity * x_velocity + y_velocity * y_velocity) / 60 >= distance){
        return true;
    }
    return false;
}

Point get_dst(Point dst)
{
    if((int)dst.x == 0) return Point(dst.x + 1.5, dst.y + 0.5);
    else if((int)dst.x == width - 1) return Point(dst.x - 0.5, dst.y + 0.5);
    else if((int)dst.y == 0) return Point(dst.x + 0.5, dst.y + 1.5);
    else if((int)dst.y == height - 1) return Point(dst.x + 0.5, dst.y - 0.5);
    assert(0);
}

/*int later_interact_direction(Point dst)
{
    if((int)dst.x == 0) return 0;
    else if((int)dst.x == width - 1) return 4;
    else if((int)dst.y == 0) return 2;
    else if((int)dst.y == height - 1) return 6;
    assert(0);

}*/

/*int change_direction();

std::pair<int, std::string> Move_along_the_route(Point src, Point dst, int player_id){
    Point player_dst = get_dst(dst);
    double player_x = Players[player_id].x;
    double player_y = Players[player_id].y;
    int player_direction = Players[player_id].cur_direction;
    double x_velocity = Players[player_id].X_Velocity;
    double y_velocity = Players[player_id].Y_Velocity;
    if(x_velocity == 0 && y_velocity == 0){
        if(src.x != player_dst.x || src.y != player_dst.y){
            int nxt_direction = search_path_direction(player_id, Point(player_x, player_y), player_dst);
            if(nxt_direction == -1){
                return {0, "Move "};
            }
            else{
                return {0, "Move " + directions[nxt_direction]};
            }
        }
        else {
            return {1, "Move "};//已经到达目的地
        }
    }
    if(player_direction < 0) return {0, "Move "};//速度不为0，但是方向为-1，说明玩家正在刹车
    double nxt_x = src.x + (double)nxt[player_direction][0];
    double nxt_y = src.y + (double)nxt[player_direction][1];
    bool arriving = aboout_to_arrive(nxt_x, nxt_y, player_x, player_y, x_velocity, y_velocity);
    if(!arriving){
        if(player_direction >= 0){//继续沿着之前的方向走
            return {0, "Move " + directions[player_direction]};
        }
        else{
            //int nxt_direction = search_path_direction(player_id, Point(nxt_x, nxt_y), player_dst);
            return {0, "Move "};
        }
    }
    else{
        Players[player_id].src = Point(nxt_x, nxt_y);
        if(nxt_x == player_dst.x && nxt_y == player_dst.y){//如果到了终点格子
            return {0, "Move "};//刹车
        }
        else{
            int nxt_direction = search_path_direction(player_id, Players[player_id].src, player_dst);
            if(nxt_direction == -1){
                return {0, "Move "};
            }
            else{
                if(nxt_direction == player_direction){
                    return {0, "Move " + directions[nxt_direction]};
                }
                else{
                    return {0, "Move "};
                }
            }
        }
    }
    assert(0);
}*/

Point get_player_src(Point src, int player_id)
{
    std::pair<double, double> center = Roundabout(src.x, src.y);
    double delta_x = ABS(center.first, src.x);
    double delta_y = ABS(center.second, src.y);
    if(delta_x < 0.35 && delta_y < 0.35){
        Players[player_id].src = Point(center.first, center.second);
        Players[player_id].adjust = false;
    }
    else{
        Players[player_id].adjust = true;
    }
    return Players[player_id].src;
}

Point get_player_dst(Point dst)
{
    if((int)dst.x == 0) return Point(dst.x + 1.5, dst.y + 0.5);
    else if((int)dst.x == width - 1) return Point(dst.x - 0.5, dst.y + 0.5);
    else if((int)dst.y == 0) return Point(dst.x + 0.5, dst.y + 1.5);
    else if((int)dst.y == height - 1) return Point(dst.x + 0.5, dst.y - 0.5);
    std::cerr << "Error: get_player_dst" << std::endl;
    assert(0);
}

bool arriving(Point src, Point dst, int player_id)
{
    return src.x == dst.x && src.y == dst.y && Players[player_id].X_Velocity == 0 && Players[player_id].Y_Velocity == 0;
}

bool arrived(Point src, Point dst, int player_id)
{
    double delta_x = ABS(src.x, dst.x);
    double delta_y = ABS(src.y, dst.y);
    double distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    return distance < 0.2 && Players[player_id].X_Velocity == 0 && Players[player_id].Y_Velocity == 0;
}

std::string slightly_move(Point src, Point dst, int player_id)
{
    double delta_x = ABS(src.x, dst.x);
    double delta_y = ABS(src.y, dst.y);
    double distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    if(distance < 0.2){
        return " ";
    }
    std::string ret = "";
    if(src.x > dst.x) ret += "L";
    if(src.x < dst.x) ret += "R";
    if(src.y > dst.y) ret += "U";
    if(src.y < dst.y) ret += "D";
    return ret;

}

int change_direction(Point src, int prev, int next, int player_id)
{
    if(Players[player_id].X_Velocity == 0 && Players[player_id].Y_Velocity == 0 && !Players[player_id].adjust){
        Players[player_id].cur_direction = next;
        return next;
    }
    if(Players[player_id].adjust){//调整位置至接近中心
        std::pair<double, double> center = Roundabout(src.x, src.y);
        std::string direction = "";
        if(src.x - center.first >= 0.35) direction += "L";
        if(src.x - center.first <= -0.35) direction += "R";
        if(src.y - center.second >= 0.35) direction += "U";
        if(src.y - center.second <= -0.35) direction += "D";
        assert(direction!="");
        if(direction == "L") return 0;
        else if(direction == "LU") return 1;
        else if(direction == "U") return 2;
        else if(direction == "RU") return 3;
        else if(direction == "R") return 4;
        else if(direction == "RD") return 5;
        else if(direction == "D") return 6;
        else if(direction == "LD") return 7;
    }
    return -1;
}

std::pair<int, std::string> Move_along_the_route(Point src, Point dst, int player_id)
{
    Work player_work = player_id == 0 ? player0_work : player1_work;
    if(player_work.task_index + 1 < player_work.current_tasks.size()){
        if(player_work.current_tasks[player_work.task_index + 1].object == "Pan" && player_work.current_tasks[player_work.task_index + 1].action == Put){
            int Pan_x = static_pos["Pan"][0].x;
            int Pan_y = static_pos["Pan"][0].y;
            if(reserved[Pan_y][Pan_x] != player_id + 1 && reserved[Pan_y][Pan_x] != 0){//被另一个玩家占用
                return {0, "Move "};
            }
            else reserved[Pan_y][Pan_x] = player_id + 1;//占用位置
        }
        if(player_work.current_tasks[player_work.task_index + 1].object == "Pot" && player_work.current_tasks[player_work.task_index + 1].action == Put){
            int Pot_x = static_pos["Pot"][0].x;
            int Pot_y = static_pos["Pot"][0].y;
            if(reserved[Pot_y][Pot_x] != player_id + 1 && reserved[Pot_y][Pot_x] != 0){//被另一个玩家占用
                return {0, "Move "};
            }
            else reserved[Pot_y][Pot_x] = player_id + 1;//占用位置
        }
    }
    Point player_dst = get_player_dst(dst);
    Point player_src = get_player_src(src, player_id);
    if(arriving(player_src, player_dst, player_id)){
        Players[player_id].cur_direction = -1;
        //if(arrived(src, player_dst, player_id)){
            change_move_strategy[player_id] = false;
            return {1, "Move "};
        //}
        //else return {0, "Move " + slightly_move(src, player_dst, player_id)};
    }
    //int move_direction = search_path_direction(player_id, player_src, player_dst);
    std::pair<int, bool>pa = search_path_direction(player_id, src, player_src, player_dst);
    int move_direction = pa.first;
    bool flag = pa.second;//flag表示是否是欧几里得路线
    std::cerr << "player:"<<player_id<<" flag:" << flag <<" pos:"<<src.x<<" "<<src.y<<" dst:"<<player_dst.x<<" "<<player_dst.y<<'\n';
    if(flag){//欧几里得路线,中间没有刹车
        return {0, "Move " + directions[move_direction]};
    }
    else{//A*路线
        if(Players[player_id].cur_direction == move_direction){
            if(move_direction < 0) return {0, "Move "};
            else return {0, "Move " + directions[move_direction]};
        }
        else{
            int direction = change_direction(src, Players[player_id].cur_direction, move_direction, player_id);
            if(direction < 0) return {0, "Move "};
            else return {0, "Move " + directions[direction]};
        }
    }
    if(Players[player_id].cur_direction < 0) return {0, "Move "};
    else return {0, "Move " + directions[Players[player_id].cur_direction]};
    std::cerr << "Error: Move_along_the_route" << std::endl;
    assert(0);
}