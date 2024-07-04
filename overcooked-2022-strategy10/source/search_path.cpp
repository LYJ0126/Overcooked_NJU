//利用A*算法启发式寻路
//这里考虑8个方向的移动(要注意由于玩家(圆)有大小，有时候不能斜着走)
//每次转向前，都在快到转弯格处的时候Move刹车，直到停止后转向
//相对前一个格，若还未到下一格，在保证无碰撞(与另一玩家或车)的前提下，继续向着之前的方向走

//#include<iostream>
#include<search_path.h>
//#include<enum.h>
//#include<common.h>

struct Node open_list[25][25];//开启列表
int close_list[25][25];//关闭列表
//对于横纵坐标，搜索的时候考虑圆在方格中心

/*double H_func(Point start, Point end)//启发函数(対角距离)
{
    double dx = ABS(start.x, end.x);
    double dy = ABS(start.y, end.y);
    return (dx + dy) + (sqrt_2 - 2) * MIN(dx, dy);
}*/

double H_func(Point start, Point end)//启发函数(曼哈顿距离)
{
    return ABS(start.x, end.x) + ABS(start.y, end.y);
}

void A_star_init()
{
    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {
            open_list[i][j].F = open_list[i][j].G = open_list[i][j].H = INF;
            open_list[i][j].parent.x = open_list[i][j].parent.y = -1;
            open_list[i][j].pos.x = i + 0.5;
            open_list[i][j].pos.y = j + 0.5;

            close_list[i][j] = 0;
        }
    }
}

bool cmp(Node& a, Node& b)
{
    return a.F > b.F;
}

bool blocked(int id, int direction, int current_x, int current_y, int nxt_x, int nxt_y)
{
    if (Map[nxt_y][nxt_x] != '.') return true;
    //要注意由于玩家(圆)有大小，有时候不能斜着走
    if (Map[nxt_y][current_x] != '.') return true;
    if (Map[current_y][nxt_x] != '.') return true;
    int another_player_x = (int)Players[id ^ 1].x;
    int anohter_player_y = (int)Players[id ^ 1].y;
    //不能与另一个玩家碰撞
    if(nxt_x == another_player_x && nxt_y == anohter_player_y) return true;
    if(current_x == another_player_x && nxt_y == anohter_player_y) return true;
    if(nxt_x == another_player_x && current_y == anohter_player_y) return true;

    //下一步，不能与另一个玩家碰撞
    int another_player_direction = Players[id ^ 1].cur_direction;
    if(another_player_direction < 0) return false;//另一个玩家下一步不动，故可以经过(nxt_x, nxt_y)
    int another_player_nxt_x = another_player_x + nxt[another_player_direction][0];
    int another_player_nxt_y = anohter_player_y + nxt[another_player_direction][1];
    if(nxt_x == another_player_nxt_x && nxt_y == another_player_nxt_y) return true;
    if(current_x == another_player_nxt_x && nxt_y == another_player_nxt_y) return true;
    if(nxt_x == another_player_nxt_x && current_y == another_player_nxt_y) return true;
    return false;
}

int search_path_direction(int id, Point start, Point end)//start和point中start是玩家起始位置，end是目标位置，都没有roundabout
{
    auto sstart = Roundabout(start.x, start.y);
    auto eend = Roundabout(end.x, end.y);
    struct Point startpoint = Point(sstart.first, sstart.second);
    struct Point endpoint = Point(eend.first, eend.second);
    A_star_init();
    int start_x = (int)startpoint.x;
    int start_y = (int)startpoint.y;
    int end_x = (int)end.x;
    int end_y = (int)end.y;
    open_list[start_x][start_y] = Node(startpoint, 0.0, 0.0, 0.0, startpoint);//起点加入开启列表
    std::priority_queue<Node, std::vector<Node> ,decltype(&cmp)>pq(cmp);
    pq.push(open_list[start_x][start_y]);
    while(!pq.empty()){
        struct Node current = pq.top();
        pq.pop();
        int current_x = (int)current.pos.x;
        int current_y = (int)current.pos.y;//当前圆圈在中心块
        int parent_x = (int)current.parent.x;
        int parent_y = (int)current.parent.y;
        if(close_list[current_x][current_y] > 0) continue;//关闭列表里的点不走
        close_list[current_x][current_y] = 1;//该点加入关闭列表
        if(current_x == end_x && current_y == end_y){//到了终点
            //根据结点的parent属性，倒推得出路径
            //根据路径上起点和起点第一个走向的点，得出玩家第一步移动的方向。
            Node temp = current;
            //std::cout << '\n';
            while(!((int)temp.parent.x == start_x && (int)temp.parent.y == start_y)){
                //std::cout << "(" << temp.pos.x << "," << temp.pos.y << ") <- ";
                temp = open_list[(int)temp.parent.x][(int)temp.parent.y];
            }
            //std::cout << "(" << temp.pos.x << "," << temp.pos.y << ") <- ";
            //std::cout<<"("<<startpoint.x<<","<<startpoint.y<<")\n";
            //计算方向
            int nxt_direction = -1;
            int nxt_x = (int)temp.pos.x;
            int nxt_y = (int)temp.pos.y;
            for(int j = 0; j < 8; ++j){
                if(start_x + nxt[j][0] == nxt_x && start_y + nxt[j][1] == nxt_y) {
                    nxt_direction = j;
                    break;
                }
            }
            return nxt_direction;
        }

        for(int i = 0; i < 8; ++i){
            int nxt_x = current_x + nxt[i][0];
            int nxt_y = current_y + nxt[i][1];
            if(nxt_x >= 0 && nxt_x < width && nxt_y >=0 && nxt_y < height && close_list[nxt_x][nxt_y] == 0 && !blocked(id, i, current_x, current_y, nxt_x, nxt_y)){
                double new_G = (i & 1) ? current.G + sqrt_2 : current.G + 1.0;//横屏竖直走+1，斜着走+sqrt(2)
                if(new_G < open_list[nxt_x][nxt_y].G){
                    open_list[nxt_x][nxt_y].G = new_G;
                    open_list[nxt_x][nxt_y].H = H_func(Point((double)nxt_x + 0.5, (double)nxt_y + 0.5), endpoint);
                    open_list[nxt_x][nxt_y].F = open_list[nxt_x][nxt_y].G + open_list[nxt_x][nxt_y].H;
                    //如果转弯了(改变方向),代价要增加
                    /*if(parent_x >= 0 && parent_y >= 0){
                        if((current_y - parent_y) * (nxt_x - current_x) != (current_x - parent_x) * (nxt_y - current_y)){
                            open_list[nxt_x][nxt_y].F += 0.2;
                        }
                    }*/
                    open_list[nxt_x][nxt_y].parent = Point((double)current_x + 0.5, (double)current_y + 0.5);
                    pq.push(open_list[nxt_x][nxt_y]);
                }
            }
        }
    }
    return -1;//找不到路径
}