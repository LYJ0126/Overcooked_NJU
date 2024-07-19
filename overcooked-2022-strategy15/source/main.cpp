#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <string>
#include <framework.h>
#include <cmath>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cerr.tie(nullptr);
    std::cerr << std::nounitbuf;
    std::string s;
    std::stringstream ss;
    int frame;

    init_read();

    /*
        你可以在读入后进行一些相关预处理，时间限制：5秒钟
    */
    init();
    

    int totalFrame = 14400;
    for (int i = 0; i < totalFrame; i++)
    {
        bool skip = frame_read(i);
        if (skip) continue;

        /* wzy：上传该帧状态，改变状态机*/
        update();
        
        /* 输出当前帧的操作，此处仅作示例 */
        std::cout << "Frame " << i << "\n";
        
        std::string player0_Action, player1_Action;
        bool run0 = (player_State_cur[0]->cur_state == State_person::Running) ? 1 : 0;
        bool run1 = (player_State_cur[1]->cur_state == State_person::Running) ? 1 : 0;
        if(stuck_check()){
            if(run0 && run1){
                // player0_Action = calc_move_str({Players[0].x, Players[0].y}, {4, 5}, Inter_direct::Left, {0, 0});
                // player1_Action = "Move R";
                double dist0 = (Players[0].x - 4.5) * (Players[0].x - 4.5) + (Players[0].y - 5.5) * (Players[0].y - 5.5);
                double dist1 = (Players[1].x - 4.5) * (Players[1].x - 4.5) + (Players[1].y - 5.5) * (Players[1].y - 5.5);
                if(dist0 <= dist1){
                    player0_Action = calc_move_str1({Players[0].x, Players[0].y}, {4, 5}, Inter_direct::Left, {0, 0}, 0);
                    //player1_Action = anti_move(calc_move_str({Players[1].x, Players[1].y}, {4, 5}, Inter_direct::Left, {0, 0}));
                    player1_Action = schedule_player(1);
                } else {
                    //player0_Action = anti_move(calc_move_str({Players[0].x, Players[0].y}, {4, 5}, Inter_direct::Left, {0, 0}));
                    player0_Action = schedule_player(0);
                    player1_Action = calc_move_str1({Players[1].x, Players[1].y}, {4, 5}, Inter_direct::Left, {0, 0}, 1);
                }
            } else if(run0 && !run1){
                player0_Action = calc_move_str1({Players[0].x, Players[0].y}, {4, 5}, Inter_direct::Left, {0, 0}, 0);
                player1_Action = schedule_player(1);
            } else if(run1 && !run0){
                player0_Action = schedule_player(0);
                player1_Action = calc_move_str1({Players[1].x, Players[1].y}, {4, 5}, Inter_direct::Left, {0, 0}, 1);
            } else {
                player0_Action = schedule_player(0);
                player1_Action = schedule_player(1);
            }
        } else {
            if(player_access[1] < player_access[0]){
                //player1_Action = "Move";
                player1_Action = edge_check(1) ? calc_move_str1({Players[1].x, Players[1].y}, {4, 4}, Inter_direct::Left, {0, 0}, 1) : schedule_player(1);
                player0_Action = edge_check(0) ? calc_move_str1({Players[0].x, Players[0].y}, {4, 4}, Inter_direct::Left, {0, 0}, 0) : schedule_player(0);
            } else {
                player0_Action = edge_check(0) ? calc_move_str1({Players[0].x, Players[0].y}, {4, 5}, Inter_direct::Left, {0, 0}, 0) : schedule_player(0);
                player1_Action = edge_check(1) ? calc_move_str1({Players[1].x, Players[1].y}, {4, 5}, Inter_direct::Left, {0, 0}, 1) : schedule_player(1);
                //player1_Action = "Move";
            }
            
        }

        /* 合成一个字符串再输出，否则输出有可能会被打断 */
        std::string action = player0_Action + "\n" + player1_Action + "\n";
        std::cout << action;

        /* 不要忘记刷新输出流，否则游戏将无法及时收到响应 */
        std::cout.flush();
    }
}
