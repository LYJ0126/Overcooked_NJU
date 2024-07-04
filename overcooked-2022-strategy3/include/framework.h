#ifndef FRAMEWORK
#define FRAMEWORK

#include <string>
#include <vector>
#include <map>
#include <common.h>
#include <enum.h>

struct Ingredient
{
    int x, y, price;
    std::string name;
};

struct Recipe
{
    int time;
    std::string nameBefore, nameAfter;
    std::string kind;
};

struct Order
{
    int validFrame;
    int price;
    int frequency;
    std::vector<std::string> recipe;
};

struct Player
{
    double x, y;
    Point src;
    Point dst;
    double X_Velocity;
    double Y_Velocity;
    int cur_direction;
    bool adjust;
    int live;
    ContainerKind containerKind;
    std::vector<std::string> entity;
};

struct Entity
{
    double x, y;
    ContainerKind containerKind;
    std::vector<std::string> entity;
    int currentFrame, totalFrame;
    int sum;
};


/* 初始化时的读入。 */
void init_read();

void init();//预处理

/* 每一帧的读入；返回：是否跳帧。 */
bool frame_read(int nowFrame);

extern int width;
extern int height;
extern char Map[20 + 5][20 + 5];
extern struct Player Players[2 + 5];
extern int entityCount;
extern struct Entity Entity[20 + 5];
extern struct Order Order[20 + 5];

extern std::map<std::string, std::vector<Point>> static_pos;//食材、煎锅、煮锅以及不同方块的位置
extern std::map<std::string, std::vector<Point>> dynamic_plate_pos;//盘子和脏盘子的位置
extern std::map<std::vector<std::string>, std::vector<Task>> task_list;//任务列表

void update_dynamic_plate_pos();//更新动态盘子的位置

#endif