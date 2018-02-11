#include "Game.h"
#include "ConsolePrt.h"
#include "Search.h"
#include <vector>
#include <algorithm>
#include "GameData.h"
using std::vector;

Board emptygameboard = {
	{
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  } };

void swap3(Board &board)
{
	for (int i = 0; i < BLSIZE; i++)
		if (board[i])
			board[i] = board[i] % 2 + 1;
}

int judgeWin(Board &board)
{
	for (int i = 0; i < BSIZE; i++)
		for (int j = 0; j < BSIZE; j++)
			if (board(i,j))
			{
				int col = board(i,j);
				for (int d = 0; d < 4; d++)
				{
					int dx = i, dy = j, cnt = 0;
					do
					{
						dx += cx[d]; dy += cy[d];
						cnt++;
					} while (inBorder({ dx,dy }) && board(dx, dy) == col);
					if (cnt >= 5)
						return col;
				}
			}
	return 0;
}

void Game::make_move(Coord pos)
{
	gameboard(pos) = nowcol;
	history.push_back(pos.p());
	nowcol = nowcol % 2 + 1;
}

void Game::unmake_move()
{
	gameboard[history.back()] = 0;
	history.pop_back();
	nowcol = nowcol % 2 + 1;
}

void Game::reset()
{
	gameboard = emptygameboard;
	gamestep = 0;
	nowcol = 1;
	history.clear();
}

void Game::printWinner(int z)
{
	std::cout << std::endl;
	if (z == 1) 
		std::cout << "Black win!";
	else if (z==2) 
		std::cout << "White win!";
	else 
		std::cout << "Draw!";
	std::cout << std::endl;
}

void Game::runGame(Player &player1, Player &player2)
{
	reset();
	if (show_mode == 1) print(gameboard);
	while (gamestep < BLSIZE)
	{
		Coord c;
		if (nowcol == 1)
			c = player1.run(gameboard, nowcol);
		else
			c = player2.run(gameboard, nowcol);
		make_move(c);
		if (show_mode == 1) print(gameboard);
		else if (show_mode == 0) std::cout << c.format() << ' ';
		if (judgeWin(gameboard))
			break;
		gamestep++;
	}
	int winner = nowcol % 2 + 1;
	if (gamestep == BLSIZE) winner = 0;
	printWinner(winner);
}

void Game::runGame_selfplay(Player &player)
{
	reset();
	vector<BoardWeight> policy;
	if (show_mode==1) print(gameboard);
	while (gamestep < BLSIZE)
	{
		Coord c = player.run(gameboard, nowcol);
		make_move(c);
		policy.push_back(player.getlastPolicy());
		if (show_mode == 1) print(gameboard);
		else if (show_mode == 0) std::cout << c.format() <<' ';
		if (judgeWin(gameboard))
			break;
		gamestep++;
	}
	int winner = nowcol % 2 + 1;
	if (gamestep == BLSIZE) winner = 0;
	printWinner(winner);
	EposideTrainingData data(history, policy , winner);
	ofstream out(output_file, std::ios::app);
	data.writeString(out);
}

void Game::selfplay(Player &player)
{
	for (int i = 0; i < selfplay_count; i++)
	{
		std::cout << "game " << i << '\n';
		runGame_selfplay(player);
	}
}

void Game::match(Player &player1, Player &player2)
{
	int play_counts = 100;
	for (int i = 0; i < play_counts; i++)
	{
		std::cout << "game " << i << '\n';
		runGame(player1, player2);
	}
}

void Game::runRecord(const vector<int> &moves)
{
	reset();
	if (show_mode == 1) print(gameboard);
	for (auto move : moves)
	{
		Coord c(move);
		make_move(c);
		if (show_mode == 1) print(gameboard);
		else if (show_mode == 0) std::cout << c.format() << ' ';
		gamestep++;
	}
	int winner = nowcol % 2 + 1;
	if (gamestep == BLSIZE) winner = 0;
	printWinner(winner);
}

void Game::runFromFile(string filename)
{
	DataSeries<EposideData> datas;
	datas.readString(filename);
	for (auto &data : datas.datas)
	{
		runRecord(data.moves);
	}
}


void Game::runGomocup(Player &player)
{
	using std::cin;
	using std::cout;
	using std::endl;
	string command;
	Coord input, best;
	char dot;
	while (1) 
	{
		cin >> command;
		std::transform(command.begin(), command.end(), command.begin(), ::toupper);

		if (command == "START") {
			int size; cin >> size;
			if (size != 15 )
				cout << "ERROR only support 15*15 board" << endl;
			else
			{
				reset();
				cout << "OK" << endl;
			}
		}
		else if (command == "RESTART") {
			reset();
			cout << "OK" << endl;
		}
		else if (command == "TAKEBACK") {
			unmake_move();
			cout << "OK" << endl;
		}
		else if (command == "BEGIN") {
			best = player.run(gameboard, nowcol);
			make_move(best);
			cout << best.x + 1 << "," << best.y + 1 << endl;
		}
		else if (command == "TURN") {
			cin >> input.x >> dot >> input.y;
			input.x--; input.y--;
			if (!inBorder(input) || gameboard(input))
				cout << "ERROR invalid move" << endl;
			else {
				make_move(input);
				best = player.run(gameboard, nowcol);
				make_move(best);
				cout << best.x + 1 << "," << best.y + 1 << endl;
			}
		}
		else if (command == "BOARD") {
			int c;
			Coord m;
			stringstream ss;
			reset();

			cin >> command;
			std::transform(command.begin(), command.end(), command.begin(), ::toupper);
			while (command != "DONE") {
				ss.clear();
				ss << command;
				ss >> m.x >> dot >> m.y >> dot >> c;
				m.x--; m.y--;
				if (!inBorder(m) || gameboard(m))
					cout << "ERROR invalid move" << endl;
				else
					make_move(m);
				cin >> command;
				std::transform(command.begin(), command.end(), command.begin(), ::toupper);
			}
			best = player.run(gameboard, nowcol);
			make_move(best);
			cout << best.x + 1 << "," << best.y + 1<< endl;
		}
		else if (command == "INFO") {
			int value;
			string key;
			cin >> key;
			std::transform(key.begin(), key.end(), key.begin(), ::toupper);

			if (key == "TIMEOUT_TURN") {
				cin >> value;
				//TODO
			}
			else if (key == "TIMEOUT_MATCH") {
				cin >> value;
				//TODO
			}
			else if (key == "TIME_LEFT") {
				cin >> value;
				//TODO
			}
			else if (key == "MAX_MEMORY") {
				cin >> value;
				// TODO
			}
			else if (key == "GAME_TYPE") {
				cin >> value;
				// TODO
			}
			else if (key == "RULE") {
				cin >> value;
				// TODO
			}
			else if (key == "FOLDER") {
				string t;
				cin >> t;
			}
		}
		else if (command == "END")
			exit(0);
	}
	cout << "If you see this information, you should download a gomoku manager to run this program. Or you can modify Gmk0.config to change the mode. ";
}
