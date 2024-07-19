#include <iostream>
#include <framework.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <cmath>
#include <queue>
#include <time.h>

static int my_max(int x, int y){
    if(x > y)
        return x;
    return y;
}

class Node
{
	public:
		int x, y; // 节点所在位置	
		int F, G, H, E; // G:移动耗费。
					 // H:曼哈顿距离。 
					 // F = G + H 
		Node(int a, int b):x(a), y(b){}
		
		// 重载操作符，使优先队列以F值大小为标准维持堆 
		bool operator < (const Node &a) const
		{
			return F > a.F;
		} 
}; 

int dir[8][2] = {{1, 0}, {0, 1}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

std::priority_queue<Node> que;
bool visit[15][15];
int valF[15][15];
int path[15][15][2];

int Manhuattan(int x, int y, int x1, int y1){
	return (abs(x - x1) + abs(y - y1)) * 10;
}

bool NodeIsLegal(int x, int y, int xx, int yy, int p_no){
	if(x < 0 || x >= 10 || y < 0 || y >= 10) return false; // 判断边界 
	if(small_map[x][y] == Denied_wall) return false; // 判断障碍物 
	if(x != xx && y != yy && (small_map[x][yy] == Denied_wall || small_map[xx][y] == Denied_wall)) return false;
	if(p_no == 0){
		if(small_map[x][y] == Denied_player1 && d2_between_blocks(x, y, get_pos_x(0), get_pos_y(0)) <= 100) return false;
		if(x != xx && y != yy && (small_map[x][yy] == Denied_player1 || small_map[xx][y] == Denied_player1)) return false;
		if(player_access[p_no] == 1){
			if(small_map[x][y] == Restricted_player1 && d2_between_blocks(x, y, get_pos_x(0), get_pos_y(0)) <= 100) return false;
			if(x != xx && y != yy && 
			((small_map[x][yy] == Restricted_player1 && d2_between_blocks(x, yy, get_pos_x(0), get_pos_y(0)) <= 100)
			|| (small_map[xx][y] == Restricted_player1 && d2_between_blocks(xx, y, get_pos_x(0), get_pos_y(0)) <= 100))) return false;
		}
		
	}
	if(p_no == 1){
		if(small_map[x][y] == Denied_player0 && d2_between_blocks(x, y, get_pos_x(0), get_pos_y(0)) <= 100) return false;
		if(x != xx && y != yy && (small_map[x][yy] == Denied_player0 || small_map[xx][y] == Denied_player0)) return false;
		if(player_access[p_no] == 1){
			if(small_map[x][y] == Restricted_player0 && d2_between_blocks(x, y, get_pos_x(1), get_pos_y(1)) <= 100) return false;
			if(x != xx && y != yy && 
			((small_map[x][yy] == Restricted_player0 && d2_between_blocks(x, yy, get_pos_x(1), get_pos_y(1)) <= 100)
			|| (small_map[xx][y] == Restricted_player0 && d2_between_blocks(xx, y, get_pos_x(1), get_pos_y(1)) <= 100))) return false;
		}	
	}
	return true;
}

int calcNodeExtraCost(Node currNode, Node nextNode, Node endNode){
	return 0;
	if((path[currNode.x][currNode.y][0] == -1 && path[currNode.x][currNode.y][1] == -1) 
		|| nextNode.x == path[currNode.x][currNode.y][0] || nextNode.y == path[currNode.x][currNode.y][1]){
			return 0;
	}
	// 拐向终点的点
	if (nextNode.x == endNode.x || nextNode.y == endNode.y 
		|| (nextNode.x-endNode.x == nextNode.y-endNode.y) || (nextNode.x-endNode.x == endNode.y-nextNode.y)) {
		return 3;
	}	
	// 普通拐点
	return 6;
}

struct coor my_astar(int my_x, int my_y, int aim_x, int aim_y, coor my_speed, int p_no){
	int debug = 0;
std::cerr << my_x << ' ' << my_y << ' ' << aim_x << 'a' << aim_y << std::endl;
if(my_x == 4 && my_y == 3 && aim_x == 5 && aim_y == 1)
	debug = 1;
	memset(visit, false, sizeof(visit));
	memset(valF, 0, sizeof(valF));
	for(int j = 0; j < 10; j++){
		for(int k = 0; k < 10; k++){
			path[j][k][0] = -1;
			path[j][k][1] = -1;
		}
	}
	while(!que.empty())
		que.pop();

	Node node(my_x, my_y);
	Node endnode(aim_x, aim_y);
	node.G = 0; 
	node.H = Manhuattan(my_x, my_y, aim_x, aim_y); 
	node.H = 0;
	node.F = node.G + node.H;
	node.E = 0;
	valF[my_x][my_y] = node.F; 

	que.push(node); 

	while(!que.empty()){
		Node node_top = que.top(); que.pop(); 
		if(visit[node_top.x][node_top.y] == true)
			continue;
		visit[node_top.x][node_top.y] = true;
		if(node_top.x == aim_x && node_top.y == aim_y)
			break;
		if(debug)
			std::cerr << std::endl << node_top.x << node_top.y << std::endl;
		for(int i=0; i<8; i++){
			Node node_next(node_top.x + dir[i][0], node_top.y + dir[i][1]);
			if(NodeIsLegal(node_next.x, node_next.y, node_top.x, node_top.y, p_no) && !visit[node_next.x][node_next.y]) {
				node_next.G = node_top.G + int(sqrt(pow(dir[i][0],2)+pow(dir[i][1],2))*10); 
				node_next.H = Manhuattan(node_next.x, node_next.y, aim_x, aim_y);  
				node_next.H = 0;
				node_next.E = calcNodeExtraCost(node_top, node_next, endnode);
				node_next.F = node_next.G + node_next.H + node_next.E; 
				
				if(debug)
					std::cerr << node_next.x << node_next.y << std::endl;

				if(node_next.F < valF[node_next.x][node_next.y] || valF[node_next.x][node_next.y] == 0){
					path[node_next.x][node_next.y][0] = node_top.x;
					path[node_next.x][node_next.y][1] = node_top.y;
					valF[node_next.x][node_next.y] = node_next.F;
					que.push(node_next);
				}
			}
		}
	}
	
	if(path[aim_x][aim_y][0] == -1 || path[aim_x][aim_y][1] == -1){
		return {-100, -100};
	}
	int my_path_x[100];
	int my_path_y[100];
	int tot = 0;
	memset(my_path_x, 0, sizeof(my_path_x));
	memset(my_path_y, 0, sizeof(my_path_y));

	int x = aim_x, y = aim_y;
	int a = path[x][y][0], b = path[x][y][1];
	my_path_x[tot] = x;
	my_path_y[tot] = y;
	tot++;
	while(my_x != a || my_y != b){
		// if(player_access[p_no] == 0)
		// 	small_map[x][y] = my_max(small_map[x][y], Restricted_player0);
		// if(player_access[p_no] == 1)
		// 	small_map[x][y] = my_max(small_map[x][y], Restricted_player1);
		my_path_x[tot] = a;
		my_path_y[tot] = b;
		tot++;
		x = a;
		y = b;
		a = path[x][y][0];
		b = path[x][y][1];
	}
	assert(my_path_x[tot-1] == x && my_path_y[tot-1] == y);
	int delta_x = x - my_x;
	int delta_y = y - my_y;
	int save = 1;
	if(delta_x == 1 && delta_y == 1){
		for(int j = 2; j <= tot; j++){
			if((my_path_x[tot-j] - my_x) == (my_path_y[tot-j] - my_y)){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else if(delta_x == 1 && delta_y == 0){
		for(int j = 2; j <= tot; j++){
			if(0 == (my_path_y[tot-j] - my_y)){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else if(delta_x == 1 && delta_y == -1){
		for(int j = 2; j <= tot; j++){
			if((my_path_x[tot-j] - my_x) == (my_y - my_path_y[tot-j])){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else if(delta_x == 0 && delta_y == 1){
		for(int j = 2; j <= tot; j++){
			if((my_path_x[tot-j] - my_x) == 0){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else if(delta_x == 0 && delta_y == -1){
		for(int j = 2; j <= tot; j++){
			if((my_path_x[tot-j] - my_x) == 0){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else if(delta_x == -1 && delta_y == 1){
		for(int j = 2; j <= tot; j++){
			if((my_path_x[tot-j] - my_x) == (my_y - my_path_y[tot-j])){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else if(delta_x == -1 && delta_y == 0){
		for(int j = 2; j <= tot; j++){
			if(0 == (my_path_y[tot-j] - my_y)){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else if(delta_x == -1 && delta_y == -1){
		for(int j = 2; j <= tot; j++){
			if((my_path_x[tot-j] - my_x) == (my_path_y[tot-j] - my_y)){
				x = my_path_x[tot-j];
				y = my_path_y[tot-j];
				save = j;
			} else break;
		}
	} else assert(0);

	for(int j = 1; j <= save; j++){
		if(player_access[p_no] == 0)
			small_map[my_path_x[tot-j]][my_path_y[tot-j]] = my_max(small_map[my_path_x[tot-j]][my_path_y[tot-j]], Restricted_player0);
		if(player_access[p_no] == 1)
			small_map[my_path_x[tot-j]][my_path_y[tot-j]] = my_max(small_map[my_path_x[tot-j]][my_path_y[tot-j]], Restricted_player1);
		std::cerr << my_path_x[tot-j] << ' ' << my_path_y[tot-j]  << std::endl;
	}
		

    coor ans = {(double)x, (double)y};
	
std::cerr << ans.coor_x << ' ' << ans.coor_y  << std::endl;
    return ans;
}
