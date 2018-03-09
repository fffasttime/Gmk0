#include "Common.h"

int cfg_seed;

stringstream debug_s;
ofstream filelog;

void logOpen(string filename)
{
	filelog.open(filename);
}

void logRefrsh()
{
	string s;
	if (filelog.is_open())
	{
		while (!debug_s.eof())
		{
			getline(debug_s, s);
			filelog << s << std::endl;
		} 
	}
	debug_s.clear();
}