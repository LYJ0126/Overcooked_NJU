#ifndef __SEARCH_PATH_H__
#define __SEARCH_PATH_H__

#include<utility>
#include<common.h>
#include<queue>
#include<vector>
#include<framework.h>
//extern const double INF;

std:: pair<double, double> Roundabout(double pos_x, double pos_y);
int search_path_direction(int id, Point start, Point end);

#endif