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
    double dx = ABS(start.x, end.x);
    double dy = ABS(start.y, end.y);
    return dx + dy;
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

std::pair<int, bool> Euclidean(Point curpos, Point end, int id)//欧几里得走法,返回方向以及是否可行
{
    double dx = end.x - curpos.x;
    double dy = end.y - curpos.y;
    double another_player_x = Players[id ^ 1].x;
    double another_player_y = Players[id ^ 1].y;
    if((another_player_x-curpos.x)*(another_player_x-curpos.x)+(another_player_y-curpos.y)*(another_player_y-curpos.y)<=1.69) return {-1, false};
    if(ABS(curpos.x, end.x) <= 1.1 && ABS(curpos.y, end.y) <= 1.1){
        //if(Players[id].X_Velocity*Players[id].X_Velocity+Players[id].Y_Velocity*Players[id].Y_Velocity>=30*std::sqrt(dx*dx+dy*dy)) return {-1, true};
        change_move_strategy[id] = true;
        return {-1, false};//交给A*寻路处理
    }
    //检查这条直线路线上是否有障碍物以及另一个玩家
    //计算出这条直线，中间等分取19个点，检查这19个点是否有障碍物(19个点足够了)
    if(curpos.x == end.x){//无斜率
        double deltay = (end.y - curpos.y) / 20;
        for(int i = 1; i < 20; ++i){
            double y = curpos.y + (double)i * deltay;
            if(Map[(int)y][(int)curpos.x] != '.' || ((int)y == (int)another_player_y && (int)curpos.x == (int)another_player_x)) return {-1, false};
        }
    }
    else{
        double k = (end.y - curpos.y) / (end.x - curpos.x);
        double b = curpos.y - k * curpos.x;
        double deltax = (end.x - curpos.x) / 20;
        for(int i = 1; i < 20; ++i){
            double x = curpos.x + (double)i * deltax;
            double y = k * x + b;
            if(Map[(int)y][(int)x] != '.' || ((int)y == (int)another_player_y && (int)x == (int)another_player_x)) return {-1, false};
        }
    }
    //计算方向,计算出方向后也要进行碰撞检测,不会碰撞返回true
    if(dx == 0){
        //if(Players[id].X_Velocity*Players[id].X_Velocity+Players[id].Y_Velocity*Players[id].Y_Velocity>=25*std::sqrt(dx*dx+dy*dy)) return {-1, true};//刹车
        if(dy > 0){
            int current_x = (int)curpos.x;
            int current_y = (int)curpos.y;
            int nxt_x = current_x + nxt[6][0];
            int nxt_y = current_y + nxt[6][1];
            if(blocked(id, 6, current_x, current_y, nxt_x, nxt_y)) return {6, false};
            else return {6, true};
        }
        else{
            int current_x = (int)curpos.x;
            int current_y = (int)curpos.y;
            int nxt_x = current_x + nxt[2][0];
            int nxt_y = current_y + nxt[2][1];
            if(blocked(id, 2, current_x, current_y, nxt_x, nxt_y)) return {2, false};
            else return {2, true};
        }
    }
    else{
        double tan = dy / dx;
        //if(Players[id].X_Velocity*Players[id].X_Velocity+Players[id].Y_Velocity*Players[id].Y_Velocity>=25*std::sqrt(dx*dx+dy*dy)) return {-1, true};//刹车
        if(dx > 0){
           if(dy == 0){
                int current_x = (int)curpos.x;
                int current_y = (int)curpos.y;
                int nxt_x = current_x + nxt[4][0];
                int nxt_y = current_y + nxt[4][1];
                if(blocked(id, 4, current_x, current_y, nxt_x, nxt_y)) return {4, false};
                else return {4, true};
            }
            else if(tan > 0){
                if(tan > 2.414){//大于77.5度,选择下
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[6][0];
                    int nxt_y = current_y + nxt[6][1];
                    if(blocked(id, 6, current_x, current_y, nxt_x, nxt_y)) return {6, false};
                    else return {6, true};
                }
                else if(tan < 0.414){//小于22.5度,选择右
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[4][0];
                    int nxt_y = current_y + nxt[4][1];
                    if(blocked(id, 4, current_x, current_y, nxt_x, nxt_y)) return {4, false};
                    else return {4, true};
                }
                else{//选择右下
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[5][0];
                    int nxt_y = current_y + nxt[5][1];
                    if(blocked(id, 5, current_x, current_y, nxt_x, nxt_y)) return {5, false};
                    else return {5, true};
                }
            }
            else{
                if(tan < -2.414){//小于-77.5度,选择上
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[2][0];
                    int nxt_y = current_y + nxt[2][1];
                    if(blocked(id, 2, current_x, current_y, nxt_x, nxt_y)) return {2, false};
                    else return {2, true};
                }
                else if(tan > -0.414){//大于-22.5度,选择右
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[4][0];
                    int nxt_y = current_y + nxt[4][1];
                    if(blocked(id, 4, current_x, current_y, nxt_x, nxt_y)) return {4, false};
                    else return {4, true};
                }
                else{//选择右上
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[3][0];
                    int nxt_y = current_y + nxt[3][1];
                    if(blocked(id, 3, current_x, current_y, nxt_x, nxt_y)) return {3, false};
                    else return {3, true};
                }
            }
        }
        else{
            if(dy == 0){
                int current_x = (int)curpos.x;
                int current_y = (int)curpos.y;
                int nxt_x = current_x + nxt[0][0];
                int nxt_y = current_y + nxt[0][1];
                if(blocked(id, 0, current_x, current_y, nxt_x, nxt_y)) return {0, false};
                else return {0, true};
            }
            else if(tan > 0){
                if(tan > 2.414){//大于77.5度,选择上
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[2][0];
                    int nxt_y = current_y + nxt[2][1];
                    if(blocked(id, 2, current_x, current_y, nxt_x, nxt_y)) return {2, false};
                    else return {2, true};
                }
                else if(tan < 0.414){//小于22.5度,选择左
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[0][0];
                    int nxt_y = current_y + nxt[0][1];
                    if(blocked(id, 0, current_x, current_y, nxt_x, nxt_y)) return {0, false};
                    else return {0, true};
                }
                else{//选择左上
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[1][0];
                    int nxt_y = current_y + nxt[1][1];
                    if(blocked(id, 1, current_x, current_y, nxt_x, nxt_y)) return {1, false};
                    else return {1, true};
                }
            }
            else{
                if(tan < -2.414){//小于-77.5度,选择下
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[6][0];
                    int nxt_y = current_y + nxt[6][1];
                    if(blocked(id, 6, current_x, current_y, nxt_x, nxt_y)) return {6, false};
                    else return {6, true};
                }
                else if(tan > -0.414){//大于-22.5度,选择左
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[0][0];
                    int nxt_y = current_y + nxt[0][1];
                    if(blocked(id, 0, current_x, current_y, nxt_x, nxt_y)) return {0, false};
                    else return {0, true};
                }
                else{//选择左下
                    int current_x = (int)curpos.x;
                    int current_y = (int)curpos.y;
                    int nxt_x = current_x + nxt[7][0];
                    int nxt_y = current_y + nxt[7][1];
                    if(blocked(id, 7, current_x, current_y, nxt_x, nxt_y)) return {7, false};
                    else return {7, true};
                }
            }
        }
    }
    return {-1, false};
}

std::pair<int, bool> search_path_direction(int id, Point curpos, Point start, Point end)//start和point中start是玩家起始位置，end是目标位置，都没有roundabout
{
    //先考虑直线走法
    if(!change_move_strategy[id]){
        auto euclidean = Euclidean(curpos, end, id);
        if(euclidean.second) {
            Players[id].cur_direction = euclidean.first;
            return euclidean;
        }
    }
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
            return {nxt_direction, false};
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
                            open_list[nxt_x][nxt_y].F += 0.4;
                        }
                    }*/
                    open_list[nxt_x][nxt_y].parent = Point((double)current_x + 0.5, (double)current_y + 0.5);
                    pq.push(open_list[nxt_x][nxt_y]);
                }
            }
        }
    }
    return {-1, false};//找不到路径
}