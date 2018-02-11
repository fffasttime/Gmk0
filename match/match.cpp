#include "Common.h"
#include "Game.h"
#include "ConsolePrt.h"
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
namespace bp = boost::process;
namespace po = boost::program_options;

string exepath;
string str_display, str_cmd1, str_cmd2;
int matchcount;

int run()
{
	using std::cout;
	using std::endl;

	Game game;

	if (str_display[0] == 'b')
		game.show_mode = 1;
	else if (str_display[0] == 'n')
		game.show_mode = 2;

	game.match_count = matchcount;

	boost::filesystem::path p1(str_cmd1);
	boost::filesystem::path p2(str_cmd2);
	if (!p1.is_complete()) str_cmd1 = exepath + "/" + str_cmd1;
	if (!p2.is_complete()) str_cmd2 = exepath + "/" + str_cmd2;
	Player player1(str_cmd1, "p1");
	Player player2(str_cmd2, "p2");
	minit();
	game.match(player1, player2);
	mexit();
	
	return 0;
}

int getOptionCmdLine(int argc, char ** argv)
{
	using std::cout;
	using std::endl;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("display,d", po::value(&str_display)->default_value("move"), "m[ove]: show move, b[oard]: show board, n[o]: close")
		("program1,p", po::value(&str_cmd1)->default_value("p1.exe"), "program 1")
		("program2,P", po::value(&str_cmd2)->default_value("p2.exe"), "program 2")
		("mathcount,c", po::value(&matchcount)->default_value(50), "count of match game")
		("information,i", po::value(&cfg_show_information)->default_value(0), "0: close , 1: show correspondence detail")
		;
	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (std::exception &err)
	{
		cout << "[Error] " << err.what() << endl;
		return 1;
	}

	if (vm.count("help"))
	{
		cout << desc << endl;
		exit(0);
	}
	return 0;
}

int main(int argc, char **argv)
{
	minit();
	exepath = boost::filesystem::initial_path<boost::filesystem::path>().string();
	try
	{
		int ret = getOptionCmdLine(argc, argv);
		if (ret) return ret;
	}
	catch (...)
	{
		std::cout << "Error!" << std::endl;
		return 1;
	}
	run();
	return 0;
}
