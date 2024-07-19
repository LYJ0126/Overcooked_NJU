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

/* 按照读入顺序定义 */
extern int width, height;
extern char Map[20 + 5][20 + 5];
extern int IngredientCount;
extern struct Ingredient Ingredient[20 + 5];
extern int recipeCount;
extern struct Recipe Recipe[20 + 5];
extern int totalTime, randomizeSeed, totalOrderCount;
extern struct Order totalOrder[20 + 5];
extern int orderCount;
extern struct Order Order[20 + 5];
extern int k;
extern struct Player Players[2 + 5];
extern int entityCount;
extern struct Entity Entity[20 + 5];
extern int remainFrame, Fund;

/* wzy's API*/
struct str_pair{
    std::string str_a;
    std::string str_b;
};

struct coor{
    double coor_x;
    double coor_y;
};


struct State_node{
    State_person cur_state;
    Orient cur_orient;
    coor cur_aim_coor;
    ContainerKind cur_aim_container;
    Inter_direct cur_direct;
    bool (*check_func) (int);
    struct State_node * next;
    struct State_node * alter;
};

extern State_node *player0_State_list[20 + 5];
extern State_node *player1_State_list;
extern State_node *player_State_cur[2 + 5];
extern State_node nodes[1000];
extern int tot;
extern int clean_plate_num;

extern int move_map[8 * 10 + 5][8 * 10 + 5];
extern int small_map[15][15];
extern int player_access[2];
extern int player_workmode[2];

void init();
std::string schedule_player(int p_num);
struct coor find_Ing_table(std::string food);
struct coor find_tile(TileKind kind);
void update();
bool naive_check(int p_no);
std::string calc_inter_str(State_person cur_state, Inter_direct cur_direct);
struct coor find_entity(ContainerKind kind);
std::string calc_move_str(coor my_coor, coor aim_coor, Inter_direct cur_direct, coor my_speed, int p_no);
std::string calc_move_str1(coor my_coor, coor aim_coor, Inter_direct cur_direct, coor my_speed, int p_no);
Inter_direct calc_inter_direct(coor aim_coor);
bool pos_check(int p_no);
bool player_entity_check(int p_no);
bool player_noentity_check(int p_no);
bool player_container_check(int p_no);
bool player_nocontainer_check(int p_no);
bool stuck_check();
bool sink_noplate_check(int p_no);
std::string anti_move(std::string my_move);
bool edge_check(int p_no);
int find_recipe(std::string food);
bool cut_done_check(int p_no);
bool pot_done_check(int p_no);
bool pan_done_check(int p_no);
bool pot_start_check(int p_no);
bool pan_start_check(int p_no);
bool cut_fish_check(int p_no);
bool out_check(int p_no);
struct coor find_organizer();
struct coor find_empty_entity(ContainerKind kind);
void init_map();
void update_map();
struct coor my_astar(int my_x, int my_y, int aim_x, int aim_y, coor my_speed, int p_no);
bool coor_same_block_check(coor my_coor, coor inter_coor);
bool check_access(int xx, int yy, int p_no);
bool check_distance(int xx, int yy, int p_no, int d);
int Denied_another(int p_no);
std::string edge_handler(int p_no);
int get_pos_x(int p_no);
int get_pos_y(int p_no);
int d2_between_blocks(int x1, int y1, int x2, int y2);
bool two_dirtyplate_check(int p_no);
bool chop_free_check(int p_no);
bool pot_free_check(int p_no);
bool pan_free_check(int p_no);
int which_zone(int p_no);
int clean_plate_cnt();
bool minus_plate_check(int p_no);
bool plus_plate_check(int p_no);
bool one_dirtyplate_check(int p_no);

#endif