#include<common.h>
#include<enum.h>
#include<vector>

std:: pair<double, double> Roundabout(double pos_x, double pos_y)//取正方形中心坐标
{
    //对于一个坐标(横或纵)，取整(向下取整)，然后加上0.5(方格中心
    int x_floor = (int)pos_x;
    int y_floor = (int)pos_y;
    double x_round = x_floor + 0.5;
    double y_round = y_floor + 0.5;
    return std::make_pair(x_round, y_round);
}

std::string directions[8] = { "L", "LU", "U", "RU", "R", "RD", "D", "LD" };

std::map<TileKind, std::vector<Point>>Tile_pos;//砖块位置(垃圾桶、切菜板、灶台、洗水池、盘子洗净后出现位置、脏盘子返回位置、食材提交窗)

Point last_plate_location;//上一次与玩家交互的盘子的位置

Point plate_temp_pos;//盘子的临时位置

int nxt[8][2]={{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};//与directions对应的移动方向

Work cooking;//烹饪任务
Work washing;//洗盘子任务