#ifndef FRAMEWORK
#define FRAMEWORK

#include <string>
#include <vector>
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
    double X_Velocity;
    double Y_Velocity;
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

/* 每一帧的读入；返回：是否跳帧。 */
bool frame_read(int nowFrame);

#endif