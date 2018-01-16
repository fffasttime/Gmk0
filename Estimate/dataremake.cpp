#include <fstream>
#include <iostream>
using namespace std;

const int BSIZE=15;

int mm[BSIZE][BSIZE], moves[BSIZE*BSIZE];
float vals[BSIZE*BSIZE];

ifstream fin("gmkdata_o.txt");
ofstream fout("gmkdata.txt");

int nowcol; 

void print(int num)
{
	for (int i=0;i<BSIZE;i++)
		for (int j=0;j<BSIZE;j++)
			if (nowcol==1)
			{
				if (mm[i][j]==0)
					cout<<"0 0 ";
				else if (mm[i][j]==1)
					cout<<"1 0 ";
				else 
					cout<<"0 1 ";
			}
			else
			{
				if (mm[i][j]==0)
					cout<<"0 0 ";
				else if (mm[i][j]==2)
					cout<<"1 0 ";
				else 
					cout<<"0 1 ";
				
			}
	for (int i=0;i<BSIZE * BSIZE;i++)
		if (moves[i]==i)
			cout<<"1 ";
		else
			cout<<"0 ";
}

void calcVal(int nums, int val0)
{
	vals[nums-1]=val0;
	for (int i=nums-2;i>=0;i--)
		vals[i]=vals[i+1]*0.92;
}

int main()
{
	while (cin>>num)
	{
		nowcol=1;
		memset(mm,0,sizeof(mm));
		for (int i=0;i<num;i++)
			cin>>moves[i];
		int val; cin>>val;
		if (val==2)
			val=-1;
		calcVal(num, val);
		for (int i=0;i<num;i++)
		{
			print();
			cout<<vals[i]<<'\n';
			mm[moves[i]/15][moves[i]%15]=nowcol;
			nowcol=nowcol%2+1;
		}
	}
	return 0;
}
