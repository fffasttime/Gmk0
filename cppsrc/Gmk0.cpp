// Gmk0.cpp: Import of console program.
//
#include "Game.h"
#include "ConsolePrt.h"
#include "Search.h"
#include "NN/nn_cpp.h" 
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
namespace po = boost::program_options;
string exepath;
string network_file, output_file, str_mode, str_display;
int playout;

#if 1

int run()
{
	using std::cout;
	using std::endl;
	int mode = 0; //0 : selfplay, 1 : protrol
	if (str_mode[0] == 'p') mode = 1;

	Game game;

	if (str_display[0] == 'b')
		game.show_mode = 1;
	else if (str_display[0] == 'n')
		game.show_mode = 2;

	game.output_file = output_file;

	if (mode == 0)
	{
		Player player1(network_file, 400, 1.4f, true, true, 0.8f, 0.6f);
		cout << "selfplay data will be saved to " << output_file << endl;
		minit();
		game.selfplay(player1);
		mexit();
	}
	else
	{
		cfg_quiet = true;
		boost::filesystem::path p(network_file);
		if (!p.is_complete()) network_file=exepath + "/" + network_file;
		if (!boost::filesystem::exists(network_file))
		{
			cout << "ERROR could not find weight file " << network_file;
			return 1;
		}
		Player player1(network_file, 400, 1.4f, false);
		game.runGomocup(player1);
	}
	return 0;
}

int getOptionCmdLine(int argc, char ** argv)
{
	using std::cout;
	using std::endl;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("mode,m", po::value(&str_mode)->default_value("selfplay"), "p[rotrol], s[elfplay]")
		("network,n", po::value(&network_file)->default_value("weight.txt"), "network file")
		("output,o", po::value(&output_file)->default_value("selfplaydata.txt"), "selfplay output file")
		("display,d", po::value(&str_display)->default_value("move"), "m[ove]: show move, b[oard]: show board, n[o]: close")
		("playout,p", po::value(&playout)->default_value(400), "count of playouts")
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
		return 1;
	}

	if (vm.count("help"))
	{
		cout << desc << endl;
		exit(0);
	}
	return 0;
}

int getOptionJson()
{
	try
	{
		boost::property_tree::ptree root;
		boost::property_tree::read_json<boost::property_tree::ptree>(exepath + "/Gmk0.json", root);
		network_file = root.get<string>("network", "weight.txt");
		output_file = root.get<string>("output", "selfplaydata.txt");
		str_display = root.get<string>("display", "move");
		str_mode = root.get<string>("mode", "selfplay");
		playout = root.get<int>("playout", 400);

	}
	catch (std::exception &err)
	{
		std::cout << "[Error] " << err.what() << std::endl;
		system("pause");
		return 1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	initTransformTable();
	minit(); 
	exepath = boost::filesystem::initial_path<boost::filesystem::path>().string();
	if (argc==1 && boost::filesystem::exists(exepath + "/Gmk0.json"))
	{
		try
		{
			int ret = getOptionJson();
			if (ret) return ret;
		}
		catch (...)
		{
			std::cout << "Error!" << std::endl;
			return 1;
		}
	}
	else
	{
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
	}
	run();
	return 0;
}

#else
int main()
{
	initTransformTable();
	cfg_quiet = true;
	//string s; std::cin >> s >> s; std::cout << "OK" << std::endl;
	string path = boost::filesystem::initial_path().string();
	std::cout << path;
	Player player1(path + "/I17.txt", 800, 1.4, false, false, 0.0);
	Game game;
	game.runGomocup(player1);
	return 0;
}
#endif