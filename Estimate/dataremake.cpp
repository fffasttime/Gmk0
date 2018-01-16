#include <fstream>
#include <iostream>
#include <cstring>
using namespace std;

const int BSIZE=15;

int mm[BSIZE][BSIZE], moves[BSIZE*BSIZE];
float vals[BSIZE*BSIZE];

ifstream fin("gmkdata_.txt");
ofstream fout("gmkdata.txt");

int nowcol; 

void print(int step)
{
	for (int i=0;i<BSIZE;i++)
		for (int j=0;j<BSIZE;j++)
			if (mm[i][j]==0)
				fout<<"0 "; 
			else if (mm[i][j]==nowcol)
				fout<<"1 "; 
			else
				fout<<"0 "; 
	for (int i=0;i<BSIZE;i++)
		for (int j=0;j<BSIZE;j++)
			if (mm[i][j]==0)
				fout<<"0 "; 
			else if (mm[i][j]!=nowcol)
				fout<<"1 "; 
			else
				fout<<"0 ";
		
	for (int i=0;i<BSIZE * BSIZE;i++)
		if (moves[step]==i)
			fout<<"1 ";
		else
			fout<<"0 ";
}

void calcVal(int nums, int val0)
{
	vals[nums-1]=val0;
	for (int i=nums-2;i>=0;i--)
		vals[i]=vals[i+1]*0.92;
	for (int i=nums-2;i>=0;i--)
		if (i&1) vals[i]=-vals[i];
}

int main()
{
	int num, vcnt=0;
	while (fin>>num)
	{
		nowcol=1;
		memset(mm,0,sizeof(mm));
		for (int i=0;i<num;i++)
			fin>>moves[i];
		int val; fin>>val;
		if (val==2)
			val=-1;
		calcVal(num, val);
		for (int i=0;i<num;i++)
		{
			print(i);
			fout<<vals[i]<<'\n';
			mm[moves[i]/15][moves[i]%15]=nowcol;
			nowcol=nowcol%2+1;
		}
		vcnt+=num;
	}
	cout<<vcnt<<" total\n";
	return 0;
}

