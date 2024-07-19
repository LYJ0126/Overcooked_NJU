#include <iostream>
#include <framework.h>
#include <cassert>
#include <cstring>
#include <cmath>
#include <queue>

int move_map[8 * 10 + 5][8 * 10 + 5];
int small_map[10+5][10+5];

static int my_max(int x, int y){
    if(x > y)
        return x;
    return y;
}

static int my_min(int x, int y){
    if(x < y)
        return x;
    return y;
}

static int get_real(int big_coor, int small_coor){
    return (big_coor * 8 + small_coor);
}

static void block_denied(int x, int y, int xl, int xu, int yl, int yu){
    for(int j = xl; j < xu; j++)
        for(int k = yl; k < yu; k++)
            move_map[x*8+j][y*8+k] = my_max(Denied_wall, move_map[x*8+j][y*8+k]);
} 

static void block_restricted(int x, int y, int xl, int xu, int yl, int yu){
    for(int j = xl; j < xu; j++)
        for(int k = yl; k < yu; k++)
            move_map[x*8+j][y*8+k] = my_max(Restricted_wall, move_map[x*8+j][y*8+k]);
} 

static void deny_and_restrict(int real_xl, int real_xu, int real_yl, int real_yu, int blocker){
    int aim_denier, aim_restricter;
    if(blocker == 2){
        aim_denier = Denied_wall;
        aim_restricter = Restricted_wall;
    } else if(blocker == 0){
        aim_denier = Denied_player0;
        aim_restricter = Restricted_player0;
    } else if(blocker == 1){
        aim_denier = Denied_player1;
        aim_restricter = Restricted_player1;
    } else assert(0);

    for(int j = real_xl; j < real_xu; j++){
        for(int k = real_yl; k < real_yu; k++){
            move_map[j][k] = my_max(aim_denier, move_map[j][k]);
        }
            
    }
    int res_xl = my_max(0, real_xl-3);
    int res_yl = my_max(0, real_yl-2);
    int res_xu = my_min(80, real_xu+3);
    int res_yu = my_min(80, real_yu+2);
    for(int j = res_xl; j < res_xu; j++){
        for(int k = res_yl; k < res_yu; k++)
            move_map[j][k] = my_max(aim_restricter, move_map[j][k]);
    }
    res_xl = my_max(0, real_xl-2);
    res_yl = my_max(0, real_yl-3);
    res_xu = my_min(80, real_xu+2);
    res_yu = my_min(80, real_yu+3);
    for(int j = res_xl; j < res_xu; j++){
        for(int k = res_yl; k < res_yu; k++)
            move_map[j][k] = my_max(aim_restricter, move_map[j][k]);
    }
}

static void clear_move_map(){
    for(int j = 0; j < 80; j++)
        for(int k = 0; k < 80; k++)
            move_map[j][k] = Granted;
    for(int j = 0; j < 10; j++)
        for(int k = 0; k < 10; k++)
            small_map[j][k] = Granted;
}

static void clear_human_path(int p_no){
    for(int j = 0; j < 10; j++)
        for(int k = 0; k < 10; k++){
            if(small_map[j][k] == Restricted_player0 && p_no == 0)
                small_map[j][k] = Granted; 
            if(small_map[j][k] == Restricted_player1 && p_no == 1)
                small_map[j][k] = Granted; 
        }      
}

static void clear_human_map(){
    for(int j = 0; j < 80; j++)
        for(int k = 0; k < 80; k++){
            if(move_map[j][k] == Denied_player0 || move_map[j][k] == Denied_player1)
                move_map[j][k] = Granted;
            if(move_map[j][k] == Restricted_player0 || move_map[j][k] == Restricted_player1)
                move_map[j][k] = Granted;
        }
    for(int j = 0; j < 10; j++)
        for(int k = 0; k < 10; k++)
            if(small_map[j][k] == Denied_player0 || small_map[j][k] == Denied_player1)
                small_map[j][k] = Granted;
}

void init_map(){
    clear_move_map();
    for (int j = 0; j < height; j++)
        for (int k = 0; k < width; k++){
            if(Map[j][k] != getAbbrev(TileKind::Floor))
                deny_and_restrict(get_real(k, 0), get_real(k, 8), get_real(j, 0), get_real(j, 8), 2);
        }
    int player_x[2] = {(int)floor(Players[0].x * 8), (int)floor(Players[1].x * 8)};
    int player_y[2] = {(int)floor(Players[0].y * 8), (int)floor(Players[1].y * 8)};
    deny_and_restrict(player_x[0]-3, player_x[0]+4, player_y[0]-2, player_y[0]+3, 0);
    deny_and_restrict(player_x[0]-2, player_x[0]+3, player_y[0]-3, player_y[0]+4, 0);
    deny_and_restrict(player_x[1]-3, player_x[1]+4, player_y[1]-2, player_y[1]+3, 1);
    deny_and_restrict(player_x[1]-2, player_x[1]+3, player_y[1]-3, player_y[1]+4, 1);

    for(int j = 0; j < 10; j++)
        for(int k = 0; k < 10; k++){
            int player0count = 0, player1count = 0;
            for(int j1 = 0; j1 < 8; j1++){
                for(int k1 = 0; k1 < 8; k1++)
                    if(move_map[j*8+j1][k*8+k1] == Denied_wall){
                        small_map[j][k] = my_max(small_map[j][k], Denied_wall);
                        break;
                    } else if(move_map[j*8+j1][k*8+k1] == Denied_player0){
                        player0count++;
                    } else if(move_map[j*8+j1][k*8+k1] == Denied_player1){
                        player1count++;
                    }
            }
            if(player0count >= 50)
                small_map[j][k] = my_max(small_map[j][k], Denied_player0);
            if(player1count >= 50)
                small_map[j][k] = my_max(small_map[j][k], Denied_player1);
        }
}

void update_map(){
    clear_human_map();

    int player_x[2] = {(int)floor(Players[0].x * 8), (int)floor(Players[1].x * 8)};
    int player_y[2] = {(int)floor(Players[0].y * 8), (int)floor(Players[1].y * 8)};
    deny_and_restrict(player_x[0]-3, player_x[0]+4, player_y[0]-2, player_y[0]+3, 0);
    deny_and_restrict(player_x[0]-2, player_x[0]+3, player_y[0]-3, player_y[0]+4, 0);
    deny_and_restrict(player_x[1]-3, player_x[1]+4, player_y[1]-2, player_y[1]+3, 1);
    deny_and_restrict(player_x[1]-2, player_x[1]+3, player_y[1]-3, player_y[1]+4, 1);   

    for(int j = 0; j < 10; j++)
        for(int k = 0; k < 10; k++){
            int player0count = 0, player1count = 0;
            for(int j1 = 0; j1 < 8; j1++){
                for(int k1 = 0; k1 < 8; k1++)
                    if(move_map[j*8+j1][k*8+k1] == Denied_wall){
                        small_map[j][k] = my_max(small_map[j][k], Denied_wall);
                        break;
                    } else if(move_map[j*8+j1][k*8+k1] == Denied_player0){
                        player0count++;
                    } else if(move_map[j*8+j1][k*8+k1] == Denied_player1){
                        player1count++;
                    }
            }
            if(player0count >= 2)
                small_map[j][k] = my_max(small_map[j][k], Denied_player0);
            if(player1count >= 2)
                small_map[j][k] = my_max(small_map[j][k], Denied_player1);
        }
    for(int k = 0; k < 10; k++){
        for(int j = 0; j < 10; j++){
            std::cerr << small_map[j][k];
        }
        std::cerr <<  std::endl;
    }
}

static bool straight_path_check(int my_x, int my_y, int aim_x, int aim_y, int p_no){
    for(int i = 0; i <= 40; i++){
        int mid_x = (int)((double)my_x * i / 40 + (double)aim_x * (40-i) / 40);
        int mid_y = (int)((double)my_y * i / 40 + (double)aim_y * (40-i) / 40);
        if(check_distance(mid_x, mid_y, p_no, 0) == false)
            return false;
    }
    return true;
}

coor next_coor[2] = {{-100, -100}, {-100, -100}};
std::string calc_move_str(coor my_coor, coor aim_coor, Inter_direct cur_direct, coor my_speed, int p_no){
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

    if(coor_same_block_check(my_coor, inter_coor)){
        next_coor[p_no] = {-100, -100};
        return calc_move_str1(my_coor, aim_coor, cur_direct, my_speed, p_no);
    }
        
    int my_x = (int)floor(my_coor.coor_x * 8);
    int my_y = (int)floor(my_coor.coor_y * 8);
    int aim_x = (int)floor(inter_coor.coor_x * 8);
    int aim_y = (int)floor(inter_coor.coor_y * 8);
    int smy_x = (int)floor(my_coor.coor_x);
    int smy_y = (int)floor(my_coor.coor_y);
    int saim_x = (int)floor(inter_coor.coor_x);
    int saim_y = (int)floor(inter_coor.coor_y);

    double delta_x = 0, delta_y = 0, tan_yx = 0;
    delta_x = next_coor[p_no].coor_x + 0.5 - my_coor.coor_x;
    delta_y = next_coor[p_no].coor_y + 0.5 - my_coor.coor_y;
    double theta_x = 0, theta_y = 0;
    theta_x = inter_coor.coor_x - my_coor.coor_x;
    theta_y = inter_coor.coor_y - my_coor.coor_y;
    double theta_distance = std::sqrt(theta_x * theta_x + theta_y * theta_y);
    double gamma_x = 0, gamma_y = 0;
    gamma_x = Players[0].x - Players[1].x;
    gamma_y = Players[0].y - Players[1].y;
    double gamma_distance = std::sqrt(gamma_x * gamma_x + gamma_y * gamma_y);
    double alpha_x = 0, alpha_y = 0;
    alpha_x = Players[p_no].x - get_pos_x(p_no) - 0.5;
    alpha_y = Players[p_no].y - get_pos_y(p_no) - 0.5;
    double alpha_distance = std::sqrt(alpha_x * alpha_x + alpha_y * alpha_y);

    double distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    double speed2 = my_speed.coor_x * my_speed.coor_x + my_speed.coor_y * my_speed.coor_y;

std::cerr << next_coor[p_no].coor_x << ' ' << next_coor[p_no].coor_y << ' ' << p_no << std::endl;

    if(straight_path_check(my_x, my_y, aim_x, aim_y, p_no) && speed2 <= 100 
        && theta_distance <= 8 && gamma_distance >= 0){
 std::cerr << "Hey mate" << std::endl;
        if(speed2 <= 0.8 || next_coor[p_no].coor_x == -100){
            next_coor[p_no] = {-100, -100};
            return calc_move_str1(my_coor, aim_coor, cur_direct, my_speed, p_no);
        } else {
            return "Move";
        }
    }
    if(distance <= 0.1 && speed2 <= 0.4)
        next_coor[p_no] = {-100, -100};
    if(p_no == 0){
        for(int j = 0; j <= 10; j++){
            int sb_x = (int)(next_coor[p_no].coor_x * j / 10 + (double)smy_x * (10-j) / 10);
            int sb_y = (int)(next_coor[p_no].coor_y * j / 10 + (double)smy_y * (10-j) / 10);
            if(small_map[sb_x][sb_y] != Denied_player0 && 
                small_map[sb_x][sb_y] != Restricted_player0 && small_map[sb_x][sb_y] != Granted && 
                small_map[sb_x][sb_y] != Restricted_player1){
                next_coor[p_no] = {-100, -100};
                break;
            }    
        }
    }
    if(p_no == 1){
        for(int j = 0; j <= 10; j++){
            int sb_x = (int)(next_coor[p_no].coor_x * j / 10 + (double)smy_x * (10-j) / 10);
            int sb_y = (int)(next_coor[p_no].coor_y * j / 10 + (double)smy_y * (10-j) / 10);
            if(small_map[sb_x][sb_y] != Denied_player1 && 
                small_map[sb_x][sb_y] != Restricted_player1 && small_map[sb_x][sb_y] != Granted){
                next_coor[p_no] = {-100, -100};
                break;
            }    
        }
    }

    if(next_coor[p_no].coor_x == -100 && next_coor[p_no].coor_y == -100 && speed2 <= 1.0){
        if(alpha_distance <= 0.3)
            next_coor[p_no] = my_astar(smy_x, smy_y, saim_x, saim_y, my_speed, p_no);
        else
            next_coor[p_no] = {(double)smy_x, (double)smy_y};
    }
        

    if(next_coor[p_no].coor_x == -100 && next_coor[p_no].coor_y == -100)
        return "Move";

    return calc_move_str1(my_coor, {next_coor[p_no].coor_x-1, next_coor[p_no].coor_y}, Inter_direct::Left, my_speed, p_no);
}

std::string calc_move_str1(coor my_coor, coor aim_coor, Inter_direct cur_direct, coor my_speed, int p_no){
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

    double delta_x = 0, delta_y = 0, tan_yx = 0;
    delta_x = inter_coor.coor_x - my_coor.coor_x;
    delta_y = inter_coor.coor_y - my_coor.coor_y;

    double distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    double speed2 = my_speed.coor_x * my_speed.coor_x + my_speed.coor_y * my_speed.coor_y;

    
    if(distance <= speed2 / 24)
        return "Move";

    if(delta_x == 0){
        if(delta_y > 0)
            return "Move D";
        else 
            return "Move U";
    }
    if(delta_y == 0){
        if(delta_x > 0)
            return "Move R";
        else 
            return "Move L";
    }
    tan_yx = delta_y / delta_x;
    if((delta_y < 0) && (tan_yx >= 2.414 || tan_yx <= -2.414))
        return "Move U";
    else if((delta_y > 0) && (tan_yx >= 2.414 || tan_yx <= -2.414))
        return "Move D";
    else if((delta_x > 0) && (tan_yx >= -0.414 && tan_yx <= 0.414))
        return "Move R";
    else if((delta_x < 0) && (tan_yx >= -0.414 && tan_yx <= 0.414))
        return "Move L";
    else if((delta_x > 0) && (delta_y < 0) && (tan_yx >= -2.414 && tan_yx <= -0.414))
        return "Move RU";
    else if((delta_x > 0) && (delta_y > 0) && (tan_yx <= 2.414 && tan_yx >= 0.414))
        return "Move RD";
    else if((delta_x < 0) && (delta_y > 0) && (tan_yx >= -2.414 && tan_yx <= -0.414))
        return "Move LD";
    else if((delta_x < 0) && (delta_y < 0) && (tan_yx <= 2.414 && tan_yx >= 0.414))
        return "Move LU";
    else
        assert(0);
}
