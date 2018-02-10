#pragma once
#include "Board.h"
#include "Common.h"
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <vector>
namespace bp = boost::process;
using std::endl;
using std::vector;

extern bool cfg_show_information;

int judgeWin(Board &board);

class Player
{
private:
	bp::ipstream pipe_get;
	bp::opstream pipe_put;
	string name;
	bp::child *proc;
public:
	Player(string exec, string _name)
	{
		name = _name;
		if (!boost::filesystem::exists(exec))
		{
			std::cout << "[Error] Program " << exec << " not exists!" << endl;
			exit(1);
		}
		proc = new bp::child(exec, bp::std_out > pipe_get, bp::std_in < pipe_put);
		pipe_put << "START 15" << endl;
		if (cfg_show_information) std::cout << "match: START 15" << " ->" << name << endl;
		string s; pipe_get >> s;
		if (cfg_show_information) std::cout << name <<": " << s << endl;
		if (s != "OK")
		{
			std::cout << "[Error] program returned " << s << endl;
			exit(0);
		}
	}
	~Player()
	{
		delete proc;
	}
	void reset();
	Coord turn_move(Coord move);
	Coord begin();
	bool is_message(string s);
};

class Game
{
private:
	int gamestep;
	int nowcol;
	Board gameboard;
	std::vector<int> history;
	void make_move(Coord pos);
	void unmake_move();
	void reset();
	void printWinner(int z);
public:
	int runGame(Player &player1, Player &player2);
	void match(Player & player1, Player & player2);
	string output_file = "selfplaydata.txt";
	int show_mode = 0;
	int match_count = 50;
};
