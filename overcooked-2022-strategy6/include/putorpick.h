#ifndef __PUTORPICK_H__
#define __PUTORPICK_H__
#include<common.h>
#include<enum.h>
#include<framework.h>

std::pair<int, std::string> Put_object(Point dst, int player_id);
std::pair<int, std::string> Pick_object(Point dst, std::string object, int player_id);
#endif