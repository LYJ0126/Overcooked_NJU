#include <framework.h>
#include <cassert>
#include <cmath>

struct coor find_Ing_table(std::string food){
    struct coor ans;
    for(int j = 0; j < IngredientCount; j++)
        if(food == Ingredient[j].name){
            ans.coor_x = (double)Ingredient[j].x;
            ans.coor_y = (double)Ingredient[j].y;
            return ans;
        }
    return {8,9};
}

int find_recipe(std::string food){
    struct coor ans;
    for(int j = 0; j < recipeCount; j++)
        if(Recipe[j].nameAfter == food)
            return j;
    return -1;
}

struct coor find_tile(TileKind kind){
    char aim = getAbbrev(kind);
    struct coor ans;
    for (int j = 0; j < height; j++)
        for (int k = 0; k < width; k++)
            if(Map[j][k] == aim){
                ans.coor_x = (double)k;
                ans.coor_y = (double)j;
            }
    return ans;
}

struct coor find_entity(ContainerKind kind){
    struct coor ans = find_tile(TileKind::PlateRack);
    struct Entity cur_entity;
    for(int i = 0; i < entityCount; i++){
        cur_entity = Entity[i];
        if(cur_entity.containerKind == kind){
            ans.coor_x = cur_entity.x;
            ans.coor_y = cur_entity.y;
            if(!cur_entity.entity.empty())
                return ans;
        }
    }
    return ans;
}

struct coor find_empty_entity(ContainerKind kind){
    struct coor ans = find_tile(TileKind::PlateRack);
    struct Entity cur_entity;
    for(int i = 0; i < entityCount; i++){
        cur_entity = Entity[i];
        if(cur_entity.containerKind == kind){
            ans.coor_x = cur_entity.x;
            ans.coor_y = cur_entity.y;
            return ans;
        }
    }
    return ans;
}

struct coor find_organizer(){
    struct coor ans = {6, 9};
    struct Entity cur_entity;
    for(int i = 0; i < entityCount; i++){
        cur_entity = Entity[i];
        if(cur_entity.containerKind == ContainerKind::Plate)
            if(ans.coor_x == cur_entity.x && ans.coor_y == cur_entity.y)
                if(!cur_entity.entity.empty())
                    return ans;
    }
    return find_empty_entity(ContainerKind::Plate);
}

std::string calc_inter_str(State_person cur_state, Inter_direct cur_direct){
    std::string ans;
    switch (cur_state){
    case State_person::Interacting:
        ans = "Interact";
        break;
    case State_person::Picking:
        ans = "PutOrPick";
        break;
    default:
        assert(0);
    }
    if(cur_direct == Inter_direct::None)
        assert(0);
    else {
        ans = ans + " ";
        switch (cur_direct){
        case Inter_direct::Left:
            ans = ans + "L";
            break;
        case Inter_direct::Right:
            ans = ans + "R";
            break;
        case Inter_direct::Up:
            ans = ans + "U";
            break;
        case Inter_direct::Down:
            ans = ans + "D";
            break;
        default:
            assert(0);
        }
        return ans;
    }
}

Inter_direct calc_inter_direct(coor aim_coor){
    double aim_x = aim_coor.coor_x;
    double aim_y = aim_coor.coor_y;
    if((aim_x == 0 && aim_y == 0) || (aim_x == 0 && aim_y == height-1) 
    || (aim_x == width-1 && aim_y == 0) || (aim_x == width-1 && aim_y == height-1)){
        assert(0);
    }

    if(aim_x == 0)
        return Inter_direct::Left;
    else if(aim_x == width-1)
        return Inter_direct::Right;
    else if(aim_y == 0)
        return Inter_direct::Up;
    else if(aim_y == height-1)
        return Inter_direct::Down;
    else
        assert(0);
}

bool pos_check(int p_no){
    coor my_coor = {Players[p_no].x, Players[p_no].y};
    coor aim_coor = player_State_cur[p_no]->cur_aim_coor;
    Inter_direct cur_direct = player_State_cur[p_no]->cur_direct;
    coor inter_coor;
    switch (cur_direct){
    case Inter_direct::Left:
        inter_coor = {aim_coor.coor_x+1, aim_coor.coor_y};
        break;
    case Inter_direct::Right:
        inter_coor = {aim_coor.coor_x-1, aim_coor.coor_y};
        break;
    case Inter_direct::Up:
        inter_coor = {aim_coor.coor_x, aim_coor.coor_y+1};
        break;
    case Inter_direct::Down:
        inter_coor = {aim_coor.coor_x, aim_coor.coor_y-1};
        break;
    default:
        assert(0);
    }
    inter_coor.coor_x += 0.5;
    inter_coor.coor_y += 0.5;
    double delta_x = 0, delta_y = 0;
    delta_x = inter_coor.coor_x - my_coor.coor_x;
    delta_y = inter_coor.coor_y - my_coor.coor_y;

    double distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    if(distance <= 0.09)
        return 1;
    else   
        return 0;
}

bool player_entity_check(int p_no){
    return !(Players[p_no].entity.empty());
}

bool player_noentity_check(int p_no){
    return Players[p_no].entity.empty();
}

bool player_container_check(int p_no){
    if(Players[p_no].containerKind == ContainerKind::None)
        return 0;
    else return 1;
}

bool player_nocontainer_check(int p_no){
    if(Players[p_no].containerKind == ContainerKind::None)
        return 1;
    else return 0;
}

int my_timer[2] = {0, 0};
bool timer_check(int p_no, int time_frame){
    my_timer[p_no]++;
    if(my_timer[p_no] % time_frame == 0){
        my_timer[p_no] = 0;
        return 1;
    }
    return 0;
}

bool sink_noplate_check(int p_no){
    int cnt = 0;
    coor sink_pos = find_tile(TileKind::Sink);
    struct Entity my_entity; 
    for(int i = 0; i < entityCount; i++){
        my_entity = Entity[i];
        if(Entity[i].containerKind == ContainerKind::DirtyPlates){
            coor plate_pos = {Entity[i].x, Entity[i].y};
            if(plate_pos.coor_x == sink_pos.coor_x && plate_pos.coor_y == sink_pos.coor_y){
                cnt++;
                break;
            }
        }
    }
        
    if(cnt == 0)
        return 1;
    else{
        if(my_entity.totalFrame - my_entity.currentFrame <= 1)
            clean_plate_num++;
        return 0;
    } 
}

bool pot_start_check(int p_no){
    struct coor pot_stove = find_entity(ContainerKind::Pot);
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].x == pot_stove.coor_x && Entity[j].y == pot_stove.coor_y){
            my_entity = Entity[j];
            break;
        }
    if(my_entity.totalFrame)
        return 1;
    return 0;
}

bool pan_start_check(int p_no){
    struct coor pan_stove = find_entity(ContainerKind::Pan);
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].x == pan_stove.coor_x && Entity[j].y == pan_stove.coor_y){
            my_entity = Entity[j];
            break;
        }
    if(my_entity.totalFrame)
        return 1;
    return 0;
}

bool pot_done_check(int p_no){
    struct coor pot_stove = find_entity(ContainerKind::Pot);
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].x == pot_stove.coor_x && Entity[j].y == pot_stove.coor_y){
            my_entity = Entity[j];
            break;
        }
    if(my_entity.currentFrame >= my_entity.totalFrame)
        return 1;
    return 0;
}

bool pan_done_check(int p_no){
    struct coor pan_stove = find_entity(ContainerKind::Pan);
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].x == pan_stove.coor_x && Entity[j].y == pan_stove.coor_y){
            my_entity = Entity[j];
            break;
        }
    if(my_entity.currentFrame >= my_entity.totalFrame)
        return 1;
    return 0;
}

bool cut_done_check(int p_no){
    struct coor cutting_table = find_tile(TileKind::ChoppingStation);
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].x == cutting_table.coor_x && Entity[j].y == cutting_table.coor_y){
            my_entity = Entity[j];
            break;
        }
    if(my_entity.currentFrame >= my_entity.totalFrame)
        return 1;
    return 0;
}

int cnt[2+5] = {1, 1};
bool naive_check(int p_no){
    if(cnt[p_no]++ % 15 == 0)
        return 1;
    return 0;
}

double min_cmp(double a, double b){
    if(a < b)
        return a;
    else
        return b;
}

double max_cmp(double a, double b){
    if(a > b)
        return a;
    else
        return b;
}

bool cut_fish_check(int p_no){
    struct coor cutting_table = find_tile(TileKind::ChoppingStation);
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].x == cutting_table.coor_x && Entity[j].y == cutting_table.coor_y){
            my_entity = Entity[j];
            break;
        }
    if(my_entity.entity.size() == 1 && my_entity.entity[0] == "fish")
        return 1;
    if(Players[1].entity.size() == 1 && Players[1].entity[0] == "fish")
        return 1;
    return 0;
}

std::string anti_move(std::string my_move){
    if(my_move == "Move R")
        return "Move L";
    else if(my_move == "Move L")
        return "Move R";
    else if(my_move == "Move U")
        return "Move D";
    else if(my_move == "Move D")
        return "Move U";
    else if(my_move == "Move LU")
        return "Move RD";
    else if(my_move == "Move LD")
        return "Move RU";
    else if(my_move == "Move RU")
        return "Move LD";
    else if(my_move == "Move RD")
        return "Move LU";
    else
        assert(0);
}

bool coor_same_block_check(coor my_coor, coor inter_coor){
    if((int)floor(my_coor.coor_x) == (int)floor(inter_coor.coor_x) 
        && (int)floor(my_coor.coor_y) == (int)floor(inter_coor.coor_y))
        return true;
    return false;
}

bool check_access(int xx, int yy, int p_no){
	if(move_map[xx][yy] == Denied_wall || move_map[xx][yy] == Restricted_wall) 
		return false;
	if(p_no == 0){
		if(move_map[xx][yy] == Denied_player1 || move_map[xx][yy] == Restricted_player1) return false;
	} else if(p_no == 1){
		if(move_map[xx][yy] == Denied_player0 || move_map[xx][yy] == Restricted_player0) return false;
	}	
	return true;
}

bool check_distance(int xx, int yy, int p_no, int d){
	if(d == 0)
        if(check_access(xx, yy, p_no))
			return true;
		else return false;
	else if(d == 1){
		if(check_access(xx-1, yy, p_no) && check_access(xx+1, yy, p_no) 
			&& check_access(xx, yy-1, p_no) && check_access(xx, yy+1, p_no))
			return true;
		else return false;
	} else if(d == 2){
		if(check_access(xx-2, yy, p_no) && check_access(xx+2, yy, p_no) 
			&& check_access(xx, yy-2, p_no) && check_access(xx, yy+2, p_no) 
			&& check_access(xx+1, yy+1, p_no) && check_access(xx-1, yy-1, p_no) 
			&& check_access(xx-1, yy+1, p_no) && check_access(xx+1, yy-1, p_no))
			return true;
		else return false;
	} else if(d == 3){
		if(check_access(xx-3, yy, p_no) && check_access(xx+3, yy, p_no) 
			&& check_access(xx, yy-3, p_no) && check_access(xx, yy+3, p_no) 
			&& check_access(xx+2, yy+2, p_no) && check_access(xx-2, yy-2, p_no) 
			&& check_access(xx-2, yy+2, p_no) && check_access(xx+2, yy-2, p_no)
			&& check_access(xx+1, yy+2, p_no) && check_access(xx-1, yy-2, p_no) 
			&& check_access(xx-1, yy+2, p_no) && check_access(xx+1, yy-2, p_no)
			&& check_access(xx+2, yy+1, p_no) && check_access(xx-2, yy-1, p_no) 
			&& check_access(xx-2, yy+1, p_no) && check_access(xx+2, yy-1, p_no))
			return true;
		else return false;
	} else if(d == 4){
		if(check_access(xx-4, yy, p_no) && check_access(xx+4, yy, p_no) 
			&& check_access(xx, yy-4, p_no) && check_access(xx, yy+4, p_no) 
			&& check_access(xx+2, yy+3, p_no) && check_access(xx-2, yy-3, p_no) 
			&& check_access(xx-2, yy+3, p_no) && check_access(xx+2, yy-3, p_no)
			&& check_access(xx+3, yy+2, p_no) && check_access(xx-3, yy-2, p_no) 
			&& check_access(xx-3, yy+2, p_no) && check_access(xx+3, yy-2, p_no)
			&& check_access(xx+1, yy+3, p_no) && check_access(xx-1, yy-3, p_no) 
			&& check_access(xx-1, yy+3, p_no) && check_access(xx+1, yy-3, p_no)
			&& check_access(xx+3, yy+1, p_no) && check_access(xx-3, yy-1, p_no) 
			&& check_access(xx-3, yy+1, p_no) && check_access(xx+3, yy-1, p_no))
			return true;
		else return false;
	} else if(d == 5){
		if(check_access(xx-5, yy, p_no) && check_access(xx+5, yy, p_no) 
			&& check_access(xx, yy-5, p_no) && check_access(xx, yy+5, p_no) 
			&& check_access(xx+3, yy+3, p_no) && check_access(xx-3, yy-3, p_no) 
			&& check_access(xx-3, yy+3, p_no) && check_access(xx+3, yy-3, p_no)
			&& check_access(xx+4, yy+1, p_no) && check_access(xx-4, yy-1, p_no) 
			&& check_access(xx-4, yy+1, p_no) && check_access(xx+4, yy-1, p_no)
			&& check_access(xx+4, yy+2, p_no) && check_access(xx-4, yy-2, p_no) 
			&& check_access(xx-4, yy+2, p_no) && check_access(xx+4, yy-2, p_no)
			&& check_access(xx+4, yy+3, p_no) && check_access(xx-4, yy-3, p_no) 
			&& check_access(xx-4, yy+3, p_no) && check_access(xx+4, yy-3, p_no)
			&& check_access(xx+1, yy+4, p_no) && check_access(xx-1, yy-4, p_no) 
			&& check_access(xx-1, yy+4, p_no) && check_access(xx+1, yy-4, p_no)
			&& check_access(xx+2, yy+4, p_no) && check_access(xx-2, yy-4, p_no) 
			&& check_access(xx-2, yy+4, p_no) && check_access(xx+2, yy-4, p_no)
			&& check_access(xx+3, yy+4, p_no) && check_access(xx-3, yy-4, p_no) 
			&& check_access(xx-3, yy+4, p_no) && check_access(xx+3, yy-4, p_no))
			return true;
		else return false;
	} else if(d == 6){
		if(check_access(xx-6, yy, p_no) && check_access(xx+6, yy, p_no) 
			&& check_access(xx, yy-6, p_no) && check_access(xx, yy+6, p_no) 
			&& check_access(xx+4, yy+4, p_no) && check_access(xx-4, yy-4, p_no) 
			&& check_access(xx-4, yy+4, p_no) && check_access(xx+4, yy-4, p_no)
			&& check_access(xx+5, yy+1, p_no) && check_access(xx-5, yy-1, p_no) 
			&& check_access(xx-5, yy+1, p_no) && check_access(xx+5, yy-1, p_no)
			&& check_access(xx+5, yy+2, p_no) && check_access(xx-5, yy-2, p_no) 
			&& check_access(xx-5, yy+2, p_no) && check_access(xx+5, yy-2, p_no)
			&& check_access(xx+5, yy+3, p_no) && check_access(xx-5, yy-3, p_no) 
			&& check_access(xx-5, yy+3, p_no) && check_access(xx+5, yy-3, p_no)
			&& check_access(xx+1, yy+5, p_no) && check_access(xx-1, yy-5, p_no) 
			&& check_access(xx-1, yy+5, p_no) && check_access(xx+1, yy-5, p_no)
			&& check_access(xx+2, yy+5, p_no) && check_access(xx-2, yy-5, p_no) 
			&& check_access(xx-2, yy+5, p_no) && check_access(xx+2, yy-5, p_no)
			&& check_access(xx+3, yy+5, p_no) && check_access(xx-3, yy-5, p_no) 
			&& check_access(xx-3, yy+5, p_no) && check_access(xx+3, yy-5, p_no))
			return true;
		else return false;
	} else assert(0);
}

int Denied_another(int p_no){
    if(p_no == 1)
        return Denied_player0;
    else if(p_no == 0)
        return Denied_player1;
    else assert(0);
}

bool edge_check(int p_no){
    return 0;
    double min_coor = min_cmp(Players[p_no].x, Players[p_no].y);
    double max_coor = max_cmp(Players[p_no].x, Players[p_no].y);
    if(min_coor <= 1.4 || max_coor >= 8.6)
        return 1;
    else return 0;
}

bool stuck_check(){
    return 0;
    coor player0_coor = {Players[0].x, Players[0].y};
    coor player1_coor = {Players[1].x, Players[1].y};
    double delta_x = player1_coor.coor_x - player0_coor.coor_x;
    double delta_y = player1_coor.coor_y - player0_coor.coor_y;
    double distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    if(distance <= 0.73)
        return 1;
    else
        return 0;
}

bool out_check(int p_no){
    return 0;
    if(player_State_cur[p_no]->cur_state != State_person::Running)
        return 0;
    int wall;
    double min_coor = min_cmp(Players[p_no].x, Players[p_no].y);
    double max_coor = max_cmp(Players[p_no].x, Players[p_no].y);
    if(min_coor <= 2){
        if(min_coor == Players[p_no].x) wall = 1;
        else if(min_coor == Players[p_no].y) wall = 2;
        else assert(0);
    } else if (max_coor >= 8){
        if(max_coor == Players[p_no].x) wall = 3;
        else if(max_coor == Players[p_no].y) wall = 4;
        else assert(0);
    } else return 0;

    coor my_coor = {Players[p_no].x, Players[p_no].y};
    coor aim_coor = player_State_cur[p_no]->cur_aim_coor;
    Inter_direct cur_direct = player_State_cur[p_no]->cur_direct;
    coor inter_coor;
    switch (cur_direct){
    case Inter_direct::Left:
        inter_coor = {aim_coor.coor_x+1, aim_coor.coor_y};
        break;
    case Inter_direct::Right:
        inter_coor = {aim_coor.coor_x-1, aim_coor.coor_y};
        break;
    case Inter_direct::Up:
        inter_coor = {aim_coor.coor_x, aim_coor.coor_y+1};
        break;
    case Inter_direct::Down:
        inter_coor = {aim_coor.coor_x, aim_coor.coor_y-1};
        break;
    default:
        return 0;
    }
    inter_coor.coor_x += 0.5;
    inter_coor.coor_y += 0.5;
    double delta_x = 0, delta_y = 0;
    delta_x = inter_coor.coor_x - my_coor.coor_x;
    delta_y = inter_coor.coor_y - my_coor.coor_y;
    if(wall == 1 && delta_x > 0.6)
        return 1;
    if(wall == 2 && delta_y > 0.6)
        return 1;
    if(wall == 3 && delta_x < -0.6)
        return 1;
    if(wall == 4 && delta_y < -0.6)
        return 1;
    return 0;
}

int get_pos_x(int p_no){
    return (int)(floor(Players[p_no].x));
}

int get_pos_y(int p_no){
    return (int)(floor(Players[p_no].y));
}

int d2_between_blocks(int x1, int y1, int x2, int y2){
    return (x2-x1) * (x2-x1) + (y2-y1) * (y2-y1);
}

int chop_waiting[2] = {0, 0};
bool chop_free_check(int p_no){
    struct coor chop_pos = find_tile(TileKind::ChoppingStation);
    struct Entity my_entity;
    my_entity.x = -1;
    my_entity.y = -1;
    for(int j = 0; j < entityCount; j++){
        if(Entity[j].x == chop_pos.coor_x && Entity[j].y == chop_pos.coor_y){
            my_entity = Entity[j];
            break;
        }
    }
    if(my_entity.x == -1 && my_entity.y == -1){
        if(chop_waiting[p_no] == 0){
            chop_waiting[p_no] = 0;
            return 1;
        } else if(chop_waiting[p_no] == 1){
            int ans = timer_check(p_no, 2);
            if(ans == 1){
                chop_waiting[p_no] = 0;
                return 1;
            } else return 0;
        } else assert(0);
    } else {
        chop_waiting[p_no] = 1;
        return 0;
    }
    return 0;
}

int pot_waiting[2] = {0, 0};
bool pot_free_check(int p_no){
    struct coor pot_stove = find_entity(ContainerKind::Pot);
    struct Entity my_entity;
    my_entity.x = -1;
    my_entity.y = -1;
    for(int j = 0; j < entityCount; j++){
        if(Entity[j].x == pot_stove.coor_x && Entity[j].y == pot_stove.coor_y){
            my_entity = Entity[j];
            break;
        }
    }
    if(my_entity.entity.empty()){
        if(pot_waiting[p_no] == 0){
            pot_waiting[p_no] = 0;
            return 1;
        } else if(pot_waiting[p_no] == 1){
            return 1;
            int ans = timer_check(p_no, 2);
            if(ans == 1){
                pot_waiting[p_no] = 0;
                return 1;
            } else return 0;
        } else assert(0);
    } else {
        pot_waiting[p_no] = 1;
        return 0;
    }
    return 0;
}

int pan_waiting[2] = {0, 0};
bool pan_free_check(int p_no){
    return 1;
    struct coor pan_stove = find_entity(ContainerKind::Pan);
    struct Entity my_entity;
    my_entity.x = -1;
    my_entity.y = -1;
    for(int j = 0; j < entityCount; j++){
        if(Entity[j].x == pan_stove.coor_x && Entity[j].y == pan_stove.coor_y){
            my_entity = Entity[j];
            break;
        }
    }
    if(my_entity.entity.empty()){
        if(pan_waiting[p_no] == 0){
            pan_waiting[p_no] = 0;
            return 1;
        } else if(pan_waiting[p_no] == 1){
            int ans = timer_check(p_no, 2);
            if(ans == 1){
                pan_waiting[p_no] = 0;
                return 1;
            } else return 0;
        } else assert(0);
    } else {
        pan_waiting[p_no] = 1;
        return 0;
    }
    return 0;
}

bool two_dirtyplate_check(int p_no){
    struct coor plate_return = find_tile(TileKind::PlateReturn);
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].x == plate_return.coor_x && Entity[j].y == plate_return.coor_y){
            my_entity = Entity[j];
            break;
        }
    if(my_entity.containerKind == ContainerKind::DirtyPlates && my_entity.sum >= 1)
        return 1;
    return 0;
}

int clean_plate_cnt(){
    return clean_plate_num;
    int my_cnt = 0;
    struct Entity my_entity;
    for(int j = 0; j < entityCount; j++)
        if(Entity[j].containerKind == ContainerKind::Plate)
            my_cnt++;
    if(Players[0].containerKind == ContainerKind::Plate)
        my_cnt++;
    if(Players[1].containerKind == ContainerKind::Plate)
        my_cnt++;
    return my_cnt;
}

bool minus_plate_check(int p_no){
    clean_plate_num--;
    return 1;
}

bool plus_plate_check(int p_no){
    clean_plate_num++;
    return 1;
}
