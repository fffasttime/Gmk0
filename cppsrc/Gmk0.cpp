// Gmk0.cpp: Import of console program.
//
#include "Game.h"
#include "ConsolePrt.h"
#include "Search.h"
#include "NN/nn_cpp.h" 
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#if 0

int runOptions(int argc, char ** argv)
{
	using std::cout;
	using std::endl;
	po::options_description desc("Allowed options");
	string network1_file, network2_file, output_file, str_mode, str_display;

	desc.add_options()
		("help,h", "produce help message")
		("mode,m", po::value(&str_mode)->default_value("selfplay"), "m[atch], s[elfplay]")
		("network,n", po::value(&network1_file)->default_value("weight.txt"), "network file")
		("network2,N", po::value(&network2_file)->default_value("weight.txt"), "opp network file in match")
		("output,o", po::value(&output_file)->default_value("selfplaydata.txt"), "selfplay output file")
		("display,d", po::value(&str_display)->default_value("move"), "m[ove]: show move, b[oard]: show board, n[o]: close")
		;
	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (std::exception &err)
	{
		cout <<"[Error] "<< err.what() << endl;
		system("pause");
		return 1;
	}

	if (vm.count("help"))
	{
		cout << desc << endl;
		system("pause");
		return 0;
	}

	int mode = 0; //0 : selfplay, 1 : match
	if (str_mode[0] == 'm') mode = 1;

	Game game;

	if (str_display[0] == 'b')
		game.show_mode = 1;
	else if (str_display[0] == 'n')
		game.show_mode = 2;

	game.output_file = output_file;

	if (mode == 0)
	{
		Player player1(network1_file, 400, 1.4f, true);
		cout << "selfplay data will be saved to " << output_file <<endl;
		game.selfplay(player1);
	}
	else
	{
		Player player1(network1_file, 400, 1.4f, false);
		Player player2(network1_file, 400, 1.4f, false);
		game.match(player1, player2);
	}
	return 0;
}

int main(int argc, char **argv)
{
	initTransformTable();
	minit();
	try
	{
		int ret = runOptions(argc, argv);
		mexit();
		return ret;
	}
	catch (...)
	{
		std::cout << "Error!" << std::endl;
		mexit();
		return 1;
	}
}

#else
int main()
{
	initTransformTable();
	string s; std::cin >> s; std::cout << "OK" << std::endl << "DEBUG ";
	Player player1("NN/I17.txt");
	Game game;
	game.runGomocup(player1);
	return 0;
}
#endif