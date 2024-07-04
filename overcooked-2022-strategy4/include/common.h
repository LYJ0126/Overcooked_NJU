#ifndef __COMMON_H__
#define __COMMON_H__

#include<string>
#include<map>
#include<vector>
#include<enum.h>
//#include<framework.h>
template<typename T>
inline T ABS(T a, T b)
{
    return a < b ? b - a : a - b;
}
/*double ABS(double a, double b)
{
    return a < b ? b - a : a - b;
}

int ABS(int a, int b)
{
    return a < b ? b - a : a - b;
}*/

template<typename T>
inline T MIN(T a, T b) {return a < b ? a : b;}

template<typename T>
inline T MAX(T a, T b) {return a > b ? a : b;}

const double sqrt_2 = 1.414;

const double INF = 1e9;

//int Floor(double a) {return (int)a;}

//extern int width;
//extern int height;
//extern char Map[20 + 5][20 + 5];
//extern struct Player Players[2 + 5];

struct Point
{
    double x, y;
    Point() : x(0), y(0) {}
    Point(double _x, double _y) : x(_x), y(_y) {}
    Point(int _x, int _y) : x((double)_x), y((double)_y) {}
};

struct Node
{
    Point pos;
    Point parent;
    double G, H, F;
    Node() : pos(Point()), G(INF), H(INF), F(INF), parent(Point()) {}
    Node(Point _pos, double _G, double _H, double _F, Point _parent) : pos(_pos), G(_G), H(_H), F(_F), parent(_parent) {}
};

typedef struct Task
{
    int action;
    std::string object;
    Task() : action(Move), object("") {}
    Task(int _action, std::string _object) : action(_action), object(_object) {}
}Task;

typedef struct Work
{
    int player_id;
    std::vector<Task> current_tasks;
    int task_index;
}Work;

std::pair<double, double> Roundabout(double pos_x, double pos_y);//取正方形中心坐标
extern std::string directions[8];

extern std::map<TileKind, std::vector<Point>>Tile_pos;

extern Point last_plate_location0;//玩家0上一次盘子的位置
extern Point last_plate_location1;//玩家1上一次盘子的位置
extern Point plate_temp_pos0;//玩家0盘子的临时位置
extern Point plate_temp_pos1;//玩家1盘子的临时位置
extern Point player_initial_pos[2];//玩家初始位置

extern int nxt[8][2];

//extern Work cooking;//烹饪任务
//extern Work washing;//洗盘子任务
extern Work player0_work;//玩家0工作
extern Work player1_work;//玩家1工作

extern int player0_orderid;//玩家0订单号
extern int player1_orderid;//玩家1订单号

extern int reserved[20 + 5][20 + 5];//占用的位置,防止两个玩家抢同一个盘子

extern bool avoid_status[2];//避让状态
extern Point avoiding_pos[2];//避让去处
extern int counter;//记录卡死了几帧
extern int former_index[2];//在避让中，记录另一个玩家的前一个任务索引

#endif