#include "Common.h"

void logRefrsh()
{
	string s;
	#ifdef LOG_PRINT
	while (!fout.eof())
	{
		getline(fout, s);
		filelog << s << '\n';
	}
	fout.clear();
	#endif
}