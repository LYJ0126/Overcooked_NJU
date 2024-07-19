#include <iostream>
#include <framework.h>
#include <cassert>
#include <cstring>
#include <cmath>

#define FOOD_SORT_NUM 8
static std::string food_sort[FOOD_SORT_NUM] = {"s_rice", "s_fish", "p_fish", "c_fish", "c_kelp", "fish", "rice", "kelp"};

State_node *player_State_cur[2 + 5];
int player_workmode[2];
int player_access[2];
int clean_plate_num = 3;

static void add_head(State_person my_state, Orient my_orient, coor my_coor, 
    ContainerKind my_container, Inter_direct my_direct, bool (*my_check_func)(int), int p_no, int order_num){

    nodes[tot] = {my_state, my_orient, my_coor, my_container, my_direct, my_check_func, NULL};
    if(p_no == 1)
        player1_State_list = &nodes[tot];
    else if(p_no == 0)
        player0_State_list[order_num] = &nodes[tot];
    else assert(0);
    tot++;
}

static void add_node(State_person my_state, Orient my_orient, coor my_coor, 
    ContainerKind my_container, Inter_direct my_direct, bool (*my_check_func)(int)){

    nodes[tot] = {my_state, my_orient, my_coor, my_container, my_direct, my_check_func, NULL};
    nodes[tot-1].next = &nodes[tot];
    tot++;
}

void init(){
    player_access[0] = 0;
    player_access[1] = 1;
    player_workmode[0] = 2;
    player_workmode[1] = 2;
    for(int i = 0; i < totalOrderCount; i++)
        player0_State_list[i] = NULL;
    player1_State_list = NULL;
    int x, y;
    for(int i = 0; i < totalOrderCount; i++){
        int food_cnt = totalOrder[i].recipe.size();
        std::string food_candidate[food_cnt];
        int food_get[FOOD_SORT_NUM];
        int food_get_keep[FOOD_SORT_NUM];
        memset(food_get, 0, sizeof(food_get));
        for(int j = 0; j < food_cnt; j++)
            for(int k = 0; k < FOOD_SORT_NUM; k++)
                if(food_sort[k] == totalOrder[i].recipe[j])
                    food_get[k]++;
        for(int k = 0; k < FOOD_SORT_NUM; k++)
            food_get_keep[k] = food_get[k];
        for(int j = 0; j < food_cnt; j++)
            for(int k = 0; k < FOOD_SORT_NUM; k++)
                if(food_get[k]){
                    food_candidate[j] = food_sort[k];
                    food_get[k]--;
                    break;
                }

        for(int k = 0; k < food_cnt; k++){
            std::string food = food_candidate[k];
            
            int my_recipe = find_recipe(food);
            //std::cerr << "Hi " <<  std::endl;
            if(my_recipe >= 0){
                std::string my_ingredient = Recipe[my_recipe].nameBefore;
                std::string my_kind = Recipe[my_recipe].kind;

                int his_recipe = find_recipe(my_ingredient);
                if(his_recipe >= 0){
                    std::string his_ingredient = Recipe[his_recipe].nameBefore;
                    std::string his_kind = Recipe[his_recipe].kind;
                    if(k == 0) 
                        add_head(State_person::Running, Orient::Place, find_Ing_table(his_ingredient), 
                            ContainerKind::None, calc_inter_direct(find_Ing_table(his_ingredient)), pos_check, 0, i);
                    else
                        add_node(State_person::Running, Orient::Place, find_Ing_table(his_ingredient), 
                            ContainerKind::None, calc_inter_direct(find_Ing_table(his_ingredient)), pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_entity_check);
                    if(Recipe[his_recipe].kind == "-chop->"){
                        add_node(State_person::Waiting, Orient::None, {0, 0}, 
                            ContainerKind::None, Inter_direct::None, chop_free_check);
                        add_node(State_person::Running, Orient::Place, find_tile(TileKind::ChoppingStation), 
                            ContainerKind::None, calc_inter_direct(find_tile(TileKind::ChoppingStation)), pos_check);
                        add_node(State_person::Picking, Orient::None, {0, 0}, 
                            ContainerKind::None, Inter_direct::None, player_noentity_check);
                        add_node(State_person::Interacting, Orient::None, {0, 0}, 
                            ContainerKind::None, Inter_direct::None, cut_done_check);
                        add_node(State_person::Picking, Orient::None, {0, 0}, 
                            ContainerKind::None, Inter_direct::None, player_entity_check);
                    } 
                } else if(his_recipe == -1){
                    if(k == 0)
                        add_head(State_person::Running, Orient::Place, find_Ing_table(my_ingredient), 
                            ContainerKind::None, calc_inter_direct(find_Ing_table(my_ingredient)), pos_check, 0, i);
                    else
                        add_node(State_person::Running, Orient::Place, find_Ing_table(my_ingredient), 
                            ContainerKind::None, calc_inter_direct(find_Ing_table(my_ingredient)), pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_entity_check);
                }
            
                if(Recipe[my_recipe].kind == "-chop->"){
                    add_node(State_person::Waiting, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, chop_free_check);
                    add_node(State_person::Running, Orient::Place, find_tile(TileKind::ChoppingStation), 
                        ContainerKind::None, calc_inter_direct(find_tile(TileKind::ChoppingStation)), pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_noentity_check);
                    add_node(State_person::Interacting, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, cut_done_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_entity_check);
                    add_node(State_person::Running, Orient::Entity, {0, 0}, 
                        ContainerKind::Plate, Inter_direct::None, pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_noentity_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_container_check);
                    goto goservice;
                } else if(Recipe[my_recipe].kind == "-pot->"){
                    add_node(State_person::Waiting, Orient::None, {0, 0}, 
                            ContainerKind::None, Inter_direct::None, pot_free_check);
                    add_node(State_person::Running, Orient::Entity, {0, 0}, 
                        ContainerKind::Pot, calc_inter_direct(find_entity(ContainerKind::Pot)), pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_noentity_check);
if(k != food_cnt)
    continue;
else
    goto organizer;
                    add_node(State_person::Running, Orient::Entity, {0, 0}, 
                        ContainerKind::Plate, Inter_direct::None, pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_container_check);
                    add_node(State_person::Running, Orient::Entity, {0, 0}, 
                        ContainerKind::Pot, calc_inter_direct(find_entity(ContainerKind::Pot)), pos_check);
                    add_node(State_person::Waiting, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, pot_done_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_entity_check);
                    goto goservice;
                } else if(Recipe[my_recipe].kind == "-pan->"){
                    add_node(State_person::Waiting, Orient::None, {0, 0}, 
                            ContainerKind::None, Inter_direct::None, pan_free_check);
                    add_node(State_person::Running, Orient::Entity, {0, 0}, 
                        ContainerKind::Pan, calc_inter_direct(find_entity(ContainerKind::Pan)), pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_noentity_check);
if(k != food_cnt)
    continue;
else
    goto organizer;
                    add_node(State_person::Running, Orient::Entity, {0, 0}, 
                        ContainerKind::Plate, Inter_direct::None, pos_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_container_check);
                    add_node(State_person::Running, Orient::Entity, {0, 0}, 
                        ContainerKind::Pan, calc_inter_direct(find_entity(ContainerKind::Pan)), pos_check);
                    add_node(State_person::Waiting, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, pan_done_check);
                    add_node(State_person::Picking, Orient::None, {0, 0}, 
                        ContainerKind::None, Inter_direct::None, player_entity_check);
                    goto goservice;
                } else assert(0);
            } else if(my_recipe == -1){
                if(k == 0)
                    add_head(State_person::Running, Orient::Place, find_Ing_table(food), 
                        ContainerKind::None, calc_inter_direct(find_Ing_table(food)), pos_check, 0, i);
                else
                    add_node(State_person::Running, Orient::Place, find_Ing_table(food), 
                        ContainerKind::None, calc_inter_direct(find_Ing_table(food)), pos_check);
                add_node(State_person::Picking, Orient::None, {0, 0}, 
                    ContainerKind::None, Inter_direct::None, player_entity_check);
                add_node(State_person::Running, Orient::Entity, {0, 0}, 
                    ContainerKind::Plate, Inter_direct::None, pos_check);
                add_node(State_person::Picking, Orient::None, {0, 0}, 
                    ContainerKind::None, Inter_direct::None, player_noentity_check);
                add_node(State_person::Picking, Orient::None, {0, 0}, 
                    ContainerKind::None, Inter_direct::None, player_container_check);
                goto goservice;
            }
            
goservice:
            if(k == food_cnt-1){
                 goto organizer2;
            } else {
                add_node(State_person::Running, Orient::Place, {0, 3}, 
                    ContainerKind::None, calc_inter_direct({0, 3}), pos_check);
                add_node(State_person::Picking, Orient::None, {0, 0}, 
                    ContainerKind::None, Inter_direct::None, player_noentity_check);
            }
        }
organizer:
        add_node(State_person::Running, Orient::Organizer, {0, 0}, 
            ContainerKind::Plate, Inter_direct::None, pos_check);
        add_node(State_person::Picking, Orient::None, {0, 0}, 
            ContainerKind::None, Inter_direct::None, player_container_check);
organizer2:
        if(food_get_keep[0]){
            add_node(State_person::Running, Orient::Entity, {0, 0}, 
                ContainerKind::Pot, calc_inter_direct(find_entity(ContainerKind::Pot)), pos_check);
            add_node(State_person::Waiting, Orient::None, {0, 0}, 
                ContainerKind::None, Inter_direct::None, pot_done_check);
            add_node(State_person::Picking, Orient::None, {0, 0}, 
                ContainerKind::None, Inter_direct::None, player_entity_check);
        }
        if(food_get_keep[1]){
            add_node(State_person::Running, Orient::Entity, {0, 0}, 
                ContainerKind::Pan, calc_inter_direct(find_entity(ContainerKind::Pan)), pos_check);
            add_node(State_person::Waiting, Orient::None, {0, 0}, 
                ContainerKind::None, Inter_direct::None, pan_done_check);
            add_node(State_person::Picking, Orient::None, {0, 0}, 
                ContainerKind::None, Inter_direct::None, player_entity_check);
        }
        if(food_get_keep[2]){
            add_node(State_person::Running, Orient::Entity, {0, 0}, 
                ContainerKind::Pan, calc_inter_direct(find_entity(ContainerKind::Pan)), pos_check);
            add_node(State_person::Waiting, Orient::None, {0, 0}, 
                ContainerKind::None, Inter_direct::None, pan_done_check);
            add_node(State_person::Picking, Orient::None, {0, 0}, 
                ContainerKind::None, Inter_direct::None, player_entity_check);
        }
        add_node(State_person::Running, Orient::Place, find_tile(TileKind::ServiceWindow), 
            ContainerKind::None, calc_inter_direct(find_tile(TileKind::ServiceWindow)), pos_check);
        add_node(State_person::Picking, Orient::None, {0, 0}, 
            ContainerKind::None, Inter_direct::None, player_noentity_check);
        add_node(State_person::Waiting, Orient::None, {0, 0}, 
            ContainerKind::None, Inter_direct::None, minus_plate_check);
    }
    add_head(State_person::Running, Orient::Place, find_tile(TileKind::PlateReturn), 
        ContainerKind::None, calc_inter_direct(find_tile(TileKind::PlateReturn)), pos_check, 1, 0);
    add_node(State_person::Picking, Orient::None, {0, 0}, 
        ContainerKind::None, Inter_direct::None, player_container_check);
    add_node(State_person::Running, Orient::Place, find_tile(TileKind::Sink), 
        ContainerKind::None, calc_inter_direct(find_tile(TileKind::Sink)), pos_check);
    add_node(State_person::Picking, Orient::None, {0, 0}, 
        ContainerKind::None, Inter_direct::None, player_nocontainer_check);
    add_node(State_person::Interacting, Orient::None, {0, 0}, 
        ContainerKind::None, Inter_direct::None, sink_noplate_check);
    add_node(State_person::Waiting, Orient::None, {0, 0}, 
        ContainerKind::None, Inter_direct::None, naive_check);

    init_map();
}

bool same_recipe(std::vector<std::string> recipe0, std::vector<std::string> recipe1){
    if(recipe0.size() == recipe1.size()){
        int recipe_cnt = 0;
        for(int i = 0; i < recipe0.size(); i++)
            for(int j = 0; j < recipe1.size(); j++)
                if(recipe0[i] == recipe1[j]){
                    recipe_cnt++;
                    break;
                }
        if(recipe_cnt == recipe0.size())
            return 1;
    }
    return 0;
}

void update(){
    if(player_State_cur[0] != NULL)
        if(player_State_cur[0]->check_func(0)){
            if(player_State_cur[0]->next != NULL)
                if(player_State_cur[0]->next->cur_state != State_person::Running)
                    player_State_cur[0]->next->cur_direct = player_State_cur[0]->cur_direct;
            player_State_cur[0] = player_State_cur[0]->next;
        }
            
    if(player_State_cur[1] != NULL)
        if(player_State_cur[1]->check_func(1)){
            if(player_State_cur[1]->next != NULL)
                if(player_State_cur[1]->next->cur_state != State_person::Running)
                    player_State_cur[1]->next->cur_direct = player_State_cur[1]->cur_direct;
            player_State_cur[1] = player_State_cur[1]->next;
        }
    if(player_State_cur[0] == NULL){
        if(player_workmode[0] == 2){
            player_workmode[0] = 0;
            int aim_order = 0;
            for(int i = 0; i < totalOrderCount; i++)
                if(same_recipe(totalOrder[i].recipe, Order[aim_order].recipe) == 1)
                    player_State_cur[0] = player0_State_list[i];
        } else if(player_workmode[0] == 1 || player_workmode[0] == 0){
            if(clean_plate_cnt() + player_workmode[0] <= 1 && two_dirtyplate_check(0)){
                player_workmode[0] = 1;
                player_State_cur[0] = player1_State_list;
            } else if(player_workmode[1] == 0){
                player_workmode[0] = 0;
                int aim_order = 1;
                for(int i = 0; i < totalOrderCount; i++)
                    if(same_recipe(totalOrder[i].recipe, Order[aim_order].recipe) == 1)
                        player_State_cur[0] = player0_State_list[i];
            } else if(player_workmode[1] == 1){
                player_workmode[0] = 0;
                int aim_order = 0;
                for(int i = 0; i < totalOrderCount; i++)
                    if(same_recipe(totalOrder[i].recipe, Order[aim_order].recipe) == 1)
                        player_State_cur[0] = player0_State_list[i];
            }          
        } else assert(0);
    }
        
    if(player_State_cur[1] == NULL){
        if(player_workmode[1] == 2){
            player_workmode[1] = 0;
            int aim_order = 1;
            for(int i = 0; i < totalOrderCount; i++)
                if(same_recipe(totalOrder[i].recipe, Order[aim_order].recipe) == 1)
                    player_State_cur[1] = player0_State_list[i];
        } else if(player_workmode[1] == 1 || player_workmode[1] == 0){
            if(clean_plate_cnt() + player_workmode[0] <= 1 && two_dirtyplate_check(0)){
                player_workmode[1] = 1;
                player_State_cur[1] = player1_State_list;
            } else if(player_workmode[0] == 0){
                player_workmode[1] = 0;
                int aim_order = 1;
                for(int i = 0; i < totalOrderCount; i++)
                    if(same_recipe(totalOrder[i].recipe, Order[aim_order].recipe) == 1)
                        player_State_cur[1] = player0_State_list[i];
            } else if(player_workmode[0] == 1){
                player_workmode[1] = 0;
                int aim_order = 0;
                for(int i = 0; i < totalOrderCount; i++)
                    if(same_recipe(totalOrder[i].recipe, Order[aim_order].recipe) == 1)
                        player_State_cur[1] = player0_State_list[i];
            } 
        } else assert(0);
    }
        

    update_map();
}

std::string schedule_player(int p_num){
    coor my_coor = {Players[p_num].x, Players[p_num].y};
    coor my_speed = {Players[p_num].X_Velocity, Players[p_num].Y_Velocity};
    switch (player_State_cur[p_num]->cur_state){
    case State_person::Running:
        coor aim_coor; 
        switch (player_State_cur[p_num]->cur_orient){
        case Orient::Place:
            aim_coor = player_State_cur[p_num]->cur_aim_coor;
            break;
        case Orient::Entity:
            aim_coor = find_entity(player_State_cur[p_num]->cur_aim_container);
            player_State_cur[p_num]->cur_aim_coor = aim_coor;
            player_State_cur[p_num]->cur_direct = calc_inter_direct(aim_coor);
            break;
        case Orient::Organizer:
            aim_coor = find_organizer();
            player_State_cur[p_num]->cur_aim_coor = aim_coor;
            player_State_cur[p_num]->cur_direct = calc_inter_direct(aim_coor);
            break;
        default:
            assert(0);
        }
        return calc_move_str(my_coor, aim_coor, player_State_cur[p_num]->cur_direct, my_speed, p_num);
    case State_person::Interacting:
        return calc_inter_str(player_State_cur[p_num]->cur_state, player_State_cur[p_num]->cur_direct);
    case State_person::Picking:
        return calc_inter_str(player_State_cur[p_num]->cur_state, player_State_cur[p_num]->cur_direct);
    case State_person::Waiting:
        return "Move";
    default:
        assert(0);
    }
}

std::string edge_handler(int p_no){
    double his_delta = 0.40;
    double her_delta = 0.28;
    int ans[8];
    memset(ans, 0, sizeof(ans));
    int his_x[8] = {(int)floor((Players[p_no].x) * 8), (int)floor((Players[p_no].x+his_delta) * 8), 
        (int)floor((Players[p_no].x) * 8), (int)floor((Players[p_no].x-his_delta) * 8),
        (int)floor((Players[p_no].x+her_delta) * 8), (int)floor((Players[p_no].x+her_delta) * 8),
        (int)floor((Players[p_no].x-her_delta) * 8), (int)floor((Players[p_no].x-her_delta) * 8)};
    int his_y[8] = {(int)floor((Players[p_no].y-his_delta) * 8), (int)floor((Players[p_no].y) * 8), 
        (int)floor((Players[p_no].y+his_delta) * 8), (int)floor((Players[p_no].y) * 8),
        (int)floor((Players[p_no].y-her_delta) * 8), (int)floor((Players[p_no].y+her_delta) * 8),
        (int)floor((Players[p_no].y+her_delta) * 8), (int)floor((Players[p_no].y-her_delta) * 8)};
    int my_x = (int)floor(Players[p_no].x * 8);
    int my_y = (int)floor(Players[p_no].y * 8);
    for(int i = 0; i < 8; i++){
        if(move_map[his_x[i]][his_y[i]] == Denied_wall)
            ans[i] = 1;
    }
    if(ans[0] == 0 && ans[1] == 0 && ans[2] == 0 && ans[3] == 0){
        if(ans[4] == 0 && ans[5] == 0 && ans[6] == 0 && ans[7] == 0)
            return schedule_player(p_no);
        else if(ans[4] == 1)
            return "Move LD";
        else if(ans[5] == 1)
            return "Move LU";
        else if(ans[6] == 1)
            return "Move RU";
        else if(ans[7] == 1)
            return "Move RD";
        else return schedule_player(p_no);
    } else if(ans[0] == 0 && ans[1] == 0 && ans[2] == 0 && ans[3] == 1)
        return "Move R";
    else if(ans[0] == 0 && ans[1] == 0 && ans[2] == 1 && ans[3] == 0)
        return "Move U";
    else if(ans[0] == 0 && ans[1] == 0 && ans[2] == 1 && ans[3] == 1)
        return "Move RU";
    else if(ans[0] == 0 && ans[1] == 1 && ans[2] == 0 && ans[3] == 0)
        return "Move L";
    else if(ans[0] == 0 && ans[1] == 1 && ans[2] == 0 && ans[3] == 1)
        return schedule_player(p_no);
    else if(ans[0] == 0 && ans[1] == 1 && ans[2] == 1 && ans[3] == 0)
        return "Move LU";
    else if(ans[0] == 0 && ans[1] == 1 && ans[2] == 1 && ans[3] == 1)
        return "Move U";
    else if(ans[0] == 1 && ans[1] == 0 && ans[2] == 0 && ans[3] == 0)
        return "Move D";
    else if(ans[0] == 1 && ans[1] == 0 && ans[2] == 0 && ans[3] == 1)
        return "Move RD";
    else if(ans[0] == 1 && ans[1] == 0 && ans[2] == 1 && ans[3] == 0)
        return schedule_player(p_no);
    else if(ans[0] == 1 && ans[1] == 0 && ans[2] == 1 && ans[3] == 1)
        return "Move R";
    else if(ans[0] == 1 && ans[1] == 1 && ans[2] == 0 && ans[3] == 0)
        return "Move LD";
    else if(ans[0] == 1 && ans[1] == 1 && ans[2] == 0 && ans[3] == 1)
        return "Move D";
    else if(ans[0] == 1 && ans[1] == 1 && ans[2] == 1 && ans[3] == 0)
        return "Move L";
    else if(ans[0] == 1 && ans[1] == 1 && ans[2] == 1 && ans[3] == 1)
        return "Move R";
    else assert(0);
}
