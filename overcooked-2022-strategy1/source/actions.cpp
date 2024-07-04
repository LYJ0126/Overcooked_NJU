#include<iostream>
#include<actions.h>

void update_task()
{
    if(washing.task_index == -1){//上一个洗盘子任务已经完成
        washing.task_index = 0;
        /*if(dynamic_plate_pos.find("DirtyPlates") != dynamic_plate_pos.end()){
            washing.current_tasks = task_list[{"wash"}];
        }
        else{
            //std::cout<<"return_to_platereturn\n";
            washing.current_tasks = task_list[{"return_to_platereturn"}];
        }*/
        if(dynamic_plate_pos["DirtyPlates"].size() > 0){//有脏盘子
            washing.current_tasks = task_list[{"wash"}];
        }
        else{
            washing.current_tasks = task_list[{"return_to_platereturn"}];
        }
    }
    if(cooking.task_index == -1){//上一个烹饪任务已经完成
        cooking.task_index = 0;
        /*if(dynamic_plate_pos.find("Plate") != dynamic_plate_pos.end()){
            cooking.current_tasks = task_list[Order[0].recipe];
        }
        else{
            cooking.current_tasks = task_list[{"wait"}];
        }*/
        if(dynamic_plate_pos["Plate"].size() > 0){//有盘子
            cooking.current_tasks = task_list[Order[0].recipe];
        }
        else{
            cooking.current_tasks = task_list[{"wait"}];
        }
    }
}

Point get_cook_dst()
{
    std::string cook_object = cooking.current_tasks[cooking.task_index].object;
    Point cook_dst;
    if(cook_object == "fish" || cook_object == "rice" || cook_object == "kelp"){
        cook_dst = static_pos[cook_object][0];
    }
    else if(cook_object == "Plate"){
        if(last_plate_location.x < 0 || last_plate_location.y < 0){
            //直接找一个空盘子
            for(int i = 0; i < entityCount; i++){
                if(Entity[i].entity[0] == "Plate"){
                   if(Entity[i].entity.size() == 1){//空盘子
                       Point Plate_pos = Point(Entity[i].x, Entity[i].y);
                       last_plate_location = Plate_pos;
                       cook_dst = Plate_pos;
                       break;
                   }
                }
            }
        }
        else{
            cook_dst = last_plate_location;//上一次放盘子的位置
        }
    }
    else if(cook_object == "Pan" || cook_object == "Pot"){
        cook_dst = static_pos[cook_object][0];
    }
    else if(cook_object == "ChoppingStation"){
        cook_dst = static_pos["ChoppingStation"][0];
    }
    else if(cook_object == "temp_plate_pos"){
        cook_dst = plate_temp_pos;
        last_plate_location = plate_temp_pos;
    }
    else if(cook_object == "ServiceWindow"){
        cook_dst = static_pos["ServiceWindow"][0];
    }
    else{
        std::cerr << "Error: Invalid object in get_cook_dst\n";
        assert(0);
    }
    return cook_dst;
}

Task get_cook_task()
{
    Point dst = get_cook_dst();
    cooking.player_id = 0;
    Players[0].dst = dst;
    Task task = cooking.current_tasks[cooking.task_index];
    return task;
}

Point get_wash_dst()
{
    std::string wash_object = washing.current_tasks[washing.task_index].object;
    Point wash_dst;
    if(wash_object == "PlateReturn"){
        wash_dst = static_pos["PlateReturn"][0];
    }
    else{//Sink
        wash_dst = static_pos["Sink"][0];
    }
    return wash_dst;
}

Task get_wash_task()
{
    Point dst = get_wash_dst();
    washing.player_id = 1;
    Players[1].dst = dst;
    Task task = washing.current_tasks[washing.task_index];
    return task;
}

std::string deal_with_task(Task task, int player_id)
{
    std::string player_action;
    std::pair<int, std::string>res;//玩家的状态和操作
    switch (task.action)
    {
        case Move:
            res = Move_along_the_route(Point(Players[player_id].x, Players[player_id].y), Players[player_id].dst, player_id);
            //res = {0, "Move U"};
            player_action = res.second;
            break;
        case Interact:
            res = Interact_with_object(Players[player_id].dst);
            player_action = res.second;
            break;
        case Put:
            res = Put_object(Players[player_id].dst, player_id);
            player_action = res.second;
            break;
        case Pick:
            res = Pick_object(Players[player_id].dst, task.object, player_id);
            player_action = res.second;
            break;
        default:
            std::cerr << "Error: Invalid action in deal_with_task\n";
            assert(0);
            break;
    }
    if(res.first == 1){//任务完成
        Players[player_id].status = available;
        if(player_id == 0) cooking.task_index++;
        else washing.task_index++;
    }
    if(cooking.task_index == cooking.current_tasks.size()) {
        cooking.task_index = -1;
        last_plate_location = Point(-1, -1);
    }
    if(washing.task_index == washing.current_tasks.size()) washing.task_index = -1;
    return player_action;
}

std::pair<std::string, std::string> nxtaction()
{
    /*std::string player0_action = "Move LU";
    std::string player1_action = "Move RD";
    return std::make_pair(player0_action, player1_action);*/
    update_task();
    Task player0_task = get_cook_task();
    Task player1_task = get_wash_task();
    //cooking.player_id = 0;
    //washing.player_id = 1;
    std::string player0_action = deal_with_task(player0_task, 0);
    std::string player1_action = deal_with_task(player1_task, 1);
    //std::string player0_action = "Move U";
    //std::string player1_action = "Move U";
    return std::make_pair(player0_action, player1_action);
}