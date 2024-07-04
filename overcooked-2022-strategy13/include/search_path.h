#ifndef __SEARCH_PATH_H__
#define __SEARCH_PATH_H__

#include<utility>
#include<common.h>
#include<queue>
#include<vector>
#include<framework.h>
#include<cmath>
//extern const double INF;

std:: pair<double, double> Roundabout(double pos_x, double pos_y);
std::pair<int, bool> search_path_direction(int id, Point curpos, Point start, Point end);

#endif