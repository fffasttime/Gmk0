#pragma once
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::string;

#define BSIZE (15)
#define BLSIZE (BSIZE * BSIZE)
#define FLOAT_INF (1e10)

extern int cfg_seed;

extern stringstream debug_s;

void logOpen(string filename);
void logRefrsh();

struct Coord
{
	int x, y;
	Coord() = default;
	Coord(int _x, int _y) :x(_x), y(_y) {}
	Coord(int n):x(n/ BSIZE),y(n%BSIZE){}
	int p() { return x*BSIZE + y; }
	string format() 
	{ 
		stringstream ss; 
		ss << (char)('A' + x) << y + 1; 
		string s; ss >> s; 
		return s; 
	}
};

const int C_E = 0, C_B = 1, C_W = 2;

//#define ASSERT(expr)
#define ASSERT(expr) assert(expr)

#define POLICY_MAX_EXPANDS BLSIZE