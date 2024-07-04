#include<iostream>
#include<actions.h>

void update_task()
{
    /*if(washing.task_index == -1){//上一个洗盘子任务已经完成
        washing.task_index = 0;
        if(dynamic_plate_pos["DirtyPlates"].size() > 0){//有脏盘子
            washing.current_tasks = task_list[{"wash"}];
        }
        else{
            washing.current_tasks = task_list[{"return_to_platereturn"}];
        }
    }
    if(cooking.task_index == -1){//上一个烹饪任务已经完成
        cooking.task_index = 0;
        if(dynamic_plate_pos["Plate"].size() > 0){//有盘子
            cooking.current_tasks = task_list[Order[0].recipe];
        }
        else{
            cooking.current_tasks = task_list[{"wait"}];
        }
    }*/
    if(player0_work.task_index == -1){//玩家0上一个任务已经完成
        player0_work.task_index = 0;
        //给玩家0分配任务
        if(Players[0].entity.size() > 0){//手上有盘子，处于待提交状态
            if(player0_orderid < player1_orderid){//若玩家此时订单号较小，则为优先提交订单
                player0_work.current_tasks = task_list[{"submit"}];
            }
            else{//等待
                player0_work.current_tasks = task_list[{"wait"}];
            }
        }
        else {//刚提交完订单或刚洗完盘子
            //看看有没有空的且没被预定的盘子
            bool flag = false;
            for(int i = 0; i <entityCount; ++i){
                if(Entity[i].entity[0] == "Plate"){
                    if(Entity[i].entity.size() == 1){//空盘子
                        if(reserved[(int)Entity[i].y][(int)Entity[i].x] == 0){//没被预定
                            flag = true;
                            reserved[(int)Entity[i].y][(int)Entity[i].x] = 1;//预定
                            last_plate_location0 = Point(Entity[i].x, Entity[i].y);
                            break;
                        }
                    }
                }
            }
            if(flag){//有空盘子
                player0_work.current_tasks = task_list[Order[player0_orderid].recipe];
            }
            else{
                //看看有没有脏盘子,要注意，脏盘子也要预定，否则两个玩家会抢脏盘子
                bool flag = false;
                Point plate_return_pos = static_pos["PlateReturn"][0];
                for(int i = 0; i <entityCount; ++i){
                    if(Entity[i].entity[0] == "DirtyPlates" && Entity[i].x == plate_return_pos.x && Entity[i].y == plate_return_pos.y){
                        if(reserved[(int)Entity[i].y][(int)Entity[i].x] == 0){//没被预定
                            flag = true;
                            reserved[(int)Entity[i].y][(int)Entity[i].x] = 1;//预定
                            break;
                        }
                    }
                }
                if(flag){//有脏盘子
                    player0_work.current_tasks = task_list[{"wash"}];
                }
                else{
                    player0_work.current_tasks = task_list[{"return_to_temp_plate_pos"}];
                }
            }
        }
    }
    if(player1_work.task_index == -1){//玩家1上一个任务已经完成
        player1_work.task_index = 0;
        //给玩家1分配任务
        if(Players[1].entity.size() > 0){//手上有盘子，处于待提交状态
            if(player1_orderid < player0_orderid){//若玩家此时订单号较小，则为优先提交订单
                player1_work.current_tasks = task_list[{"submit"}];
            }
            else{//等待
                player1_work.current_tasks = task_list[{"wait"}];
            }
        }
        else {//刚提交完订单或刚洗完盘子
            //看看有没有空的且没被预定的盘子
            bool flag = false;
            for(int i = 0; i <entityCount; ++i){
                if(Entity[i].entity[0] == "Plate"){
                    if(Entity[i].entity.size() == 1){//空盘子
                        if(reserved[(int)Entity[i].y][(int)Entity[i].x] == 0){//没被预定
                            flag = true;
                            reserved[(int)Entity[i].y][(int)Entity[i].x] = 2;//预定
                            last_plate_location1 = Point(Entity[i].x, Entity[i].y);
                            break;
                        }
                    }
                }
            }
            if(flag){//有空盘子
                player1_work.current_tasks = task_list[Order[player1_orderid].recipe];
            }
            else{
                //看看有没有脏盘子,要注意，脏盘子也要预定，否则两个玩家会抢脏盘子
                bool flag = false;
                Point plate_return_pos = static_pos["PlateReturn"][0];
                for(int i = 0; i <entityCount; ++i){
                    if(Entity[i].entity[0] == "DirtyPlates" && Entity[i].x == plate_return_pos.x && Entity[i].y == plate_return_pos.y){
                        if(reserved[(int)Entity[i].y][(int)Entity[i].x] == 0){//没被预定
                            flag = true;
                            reserved[(int)Entity[i].y][(int)Entity[i].x] = 2;//预定
                            break;
                        }
                    }
                }
                if(flag){//有脏盘子
                    player1_work.current_tasks = task_list[{"wash"}];
                }
                else{
                    player1_work.current_tasks = task_list[{"return_to_temp_plate_pos"}];
                }
            }
        }
    }
}

/*Point get_cook_dst()
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
}*/

Point get_dst(int player_id)
{
    std::string object = player_id == 0 ? player0_work.current_tasks[player0_work.task_index].object : player1_work.current_tasks[player1_work.task_index].object;
    Point dst;
    if(object == "PlateReturn"){
        dst = static_pos["PlateReturn"][0];
    }
    else if(object == "Sink"){
        dst = static_pos["Sink"][0];
    }
    else if(object == "fish" || object == "rice" || object == "kelp"){
        dst = static_pos[object][0];
    }
    else if(object == "Plate"){
        Point last_plate_location = player_id == 0 ? last_plate_location0 : last_plate_location1;
        if(last_plate_location.x < 0 || last_plate_location.y < 0){
            //这种情况不会发生
            std::cerr << "last_plate_location error\n";
            assert(0);
        }
        else{
            dst = last_plate_location;//上一次放盘子的位置
        }
    }
    else if(object == "Pan" || object == "Pot"){
        dst = static_pos[object][0];
    }
    else if(object == "ChoppingStation"){
        dst = static_pos["ChoppingStation"][0];
    }
    else if(object == "temp_plate_pos"){
        if(player_id == 0){
            dst = plate_temp_pos0;
            last_plate_location0 = plate_temp_pos0;
        }
        else{
            dst = plate_temp_pos1;
            last_plate_location1 = plate_temp_pos1;
        }
    }
    else if(object == "ServiceWindow"){
        dst = static_pos["ServiceWindow"][0];
    }
    else if(object == ""){//目标就是原地
        std::pair<double, double> center_pos = Roundabout(Players[player_id].x, Players[player_id].y);
        dst = Point(center_pos.first, center_pos.second);
    }
    else{
        std::cerr << "Error: Invalid object in get_dst\n";
        assert(0);
    }
    return dst;
}

Task get_task(int player_id)
{
    Work player_work = player_id == 0 ? player0_work : player1_work;
    Point dst = get_dst(player_id);
    Players[player_id].dst = dst;
    Task task = player_work.current_tasks[player_work.task_index];
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
        case Nullaction:
            res = {1, "Move "};
            player_action = res.second;
            break;
        default:
            std::cerr << "Error: Invalid action in deal_with_task\n";
            assert(0);
            break;
    }
    if(res.first == 1){//任务完成
        if(player_id == 0) player0_work.task_index++;
        else player1_work.task_index++;
    }
    //if(cooking.task_index == cooking.current_tasks.size()) {
    //    cooking.task_index = -1;
    //    last_plate_location = Point(-1, -1);
    //}
    //if(washing.task_index == washing.current_tasks.size()) washing.task_index = -1;
    if(player_id == 0){
        if(player0_work.task_index == player0_work.current_tasks.size()) {
            if(player0_work.current_tasks[player0_work.task_index - 1].object == "Sink" || player0_work.current_tasks[player0_work.task_index - 1].object == "ServiceWindow"){
                last_plate_location0 = Point(-1, -1);
            }
            if(player0_work.current_tasks[player0_work.task_index - 1].object == "ServiceWindow"){
                player0_orderid = 1;//提交订单时一定是0号订单，然后选择1号订单
                player1_orderid = 0;//提交订单后，另一个玩家的订单号变成0
            }
            player0_work.task_index = -1;
        }
    }
    else{
        if(player1_work.task_index == player1_work.current_tasks.size()) {
            if(player1_work.current_tasks[player1_work.task_index - 1].object == "Sink" || player1_work.current_tasks[player1_work.task_index - 1].object == "ServiceWindow"){
                last_plate_location1 = Point(-1, -1);
            }
            if(player1_work.current_tasks[player1_work.task_index - 1].object == "ServiceWindow"){
                player1_orderid = 1;//提交订单时一定是0号订单，然后选择1号订单
                player0_orderid = 0;//提交订单后，另一个玩家的订单号变成0
            }
            player1_work.task_index = -1;
        }
    }
    return player_action;
}

std::pair<std::string, std::string> nxtaction()
{
    /*std::string player0_action = "Move LU";
    std::string player1_action = "Move RD";
    return std::make_pair(player0_action, player1_action);*/
    update_task();
    //Task player0_task = get_cook_task();
    //Task player1_task = get_wash_task();
    Task player0_task = get_task(0);
    Task player1_task = get_task(1);
    //cooking.player_id get_task()
    //washing.player_id = 1;
    std::string player0_action = deal_with_task(player0_task, 0);
    std::string player1_action = deal_with_task(player1_task, 1);
    //std::string player0_action = "Move U";
    //std::string player1_action = "Move U";
    return std::make_pair(player0_action, player1_action);
}