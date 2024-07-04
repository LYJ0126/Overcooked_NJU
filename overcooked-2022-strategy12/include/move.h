#ifndef __MOVE_H__
#define __MOVE_H__
#include<common.h>
#include<enum.h>
#include<framework.h>
#include<search_path.h>

std::pair<int, std::string> Move_along_the_route(Point src, Point dst, int player_id);

#endif