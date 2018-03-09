#pragma once

#include "Common.h"
#include "Board.h"
#include <iomanip>
#include <vector>
#include <array>

struct EposideData
{
	typedef unsigned char ubyte;
	int stepcount;
	std::vector<int> moves;
	//winner
	int z;

	EposideData() = default;
	EposideData(const std::vector<int> &_moves, int _z) :moves(_moves),z(_z)
	{
		//moves.assign(_moves.begin(), _moves.end());
		stepcount = (int)moves.size();
	}
	EposideData(const std::string &s)
	{
		std::stringstream ss;
		ss << s;
		ss >> stepcount >> z;
		for (int i = 0; i < stepcount; i++)
		{
			int t; ss >> t;
			moves.push_back(t);
		}
	}

	std::string toString()
	{
		std::stringstream ss;
		ss << stepcount << ' ' << z;
		for (auto move : moves) ss << ' '<< move;
		std::string s; std::getline(ss, s);
		return s;
	}
	void writeByte(std::ofstream &out)
	{
		out << (ubyte)stepcount << (ubyte)z;
		for (auto move : moves)
			out << (ubyte)move;
	}
	void readByte(std::ifstream &in)
	{
		ubyte u1, u2;
		in >> u1 >> u2;
		stepcount = u1; z = u2;
		for (int i = 0; i < stepcount; i++)
		{
			ubyte t;
			moves.push_back(t);
		}
	}
	void readString(std::ifstream &in)
	{
		in >> stepcount >> z;
		for (int i = 0; i < stepcount; i++)
		{
			int t; in >> t;
			moves.push_back(t);
		}
	}
	void writeString(std::ofstream &out)
	{
		out << toString() << ' ';
	}
};

struct EposideTrainingData
{
	typedef unsigned char ubyte;
	typedef BoardWeight StepPolicy;
	int stepcount;
	std::vector<int> moves;
	std::vector<float> winrate;
	std::vector<StepPolicy> policy;
	int z;

	EposideTrainingData() = default;
	EposideTrainingData(const std::vector<int> &_moves, 
		const std::vector<StepPolicy> &_policy, 
		const std::vector<float> &_winrate, 
		int _z):z(_z)
	{
		policy.assign(_policy.begin(), _policy.end());
		winrate.assign(_winrate.begin(), _winrate.end());
		moves.assign(_moves.begin(), _moves.end());
		stepcount = (int)moves.size();
	}
	EposideTrainingData(const std::string &s)
	{
		std::stringstream ss;
		ss << s;
		ss >> stepcount >> z;
		for (int i = 0; i < stepcount; i++)
		{
			int t; ss >> t;
			moves.push_back(t);
		}
	}
	
	std::string toString()
	{
		std::stringstream ss;
		ss << stepcount;
		for (int i=0;i<stepcount;i++)
		{
			ss << ' '<< moves[i];
			ss << ' '<< winrate[i];
			for (int j = 0; j < POLICY_MAX_EXPANDS; j++)
				if (policy[i][j] == 0.0f)
					ss << " 0";
				else
					ss << ' ' << std::fixed<< std::setprecision(6) << policy[i][j];
		}
		ss << ' '<<z;
		std::string s; std::getline(ss,s);
		return s;
	}
	void writeByte(std::ofstream &out)
	{
		out << (ubyte)stepcount << (ubyte)z;
		for (int i = 0; i < stepcount; i++)
		{
			out << (ubyte)moves[i]; 
			out << winrate[i];
			for (int j = 0; j < POLICY_MAX_EXPANDS; j++)
				out << policy[i][j];
		}
	}
	void readByte(std::ifstream &in)
	{
		ubyte u1, u2;
		in >> u1 >> u2;
		stepcount = u1; z = u2;
		for (int i = 0; i < stepcount; i++)
		{
			ubyte t;
			moves.push_back(t);
			StepPolicy tp;
			for (int j = 0; j < POLICY_MAX_EXPANDS; j++)
				in>>tp[j];
			policy.push_back(tp);
		}
	}
	void readString(std::ifstream &in)
	{
		in >> stepcount >> z;
		for (int i = 0; i < stepcount; i++)
		{
			int t; in >> t;
			moves.push_back(t);
			StepPolicy tp;
			for (int j = 0; j < POLICY_MAX_EXPANDS; j++)
				in>>tp[j];
			policy.push_back(tp);
		}
	}
	void writeString(std::ofstream &out)
	{
		out << toString() << ' ';
	}
};

template<typename T>
struct DataSeries
{
	typedef unsigned char ubyte;
	int count=0;
	std::vector<T> datas;
	void dump(const T &t)
	{
		datas.push_back(t);
		count++;
	}
	void writeByte(std::string filename)
	{
		std::ofstream out(filename, ios::byte);
		out << (ubyte)count;
		for (auto &data : datas)
			data.writeByte(out);
	}
	void readByte(std::string filename)
	{
		std::ifstream in(filename, ios::byte);
		ubyte u1;
		in >> u1;
		count = u1;
		if (!datas.empty()) datas.clear();
		for (int i = 0; i < count; i++)
		{
			T data;
			data.readByte(in);
			datas.push_back(data);
		}
	}
	void writeString(std::string filename, bool app = true)
	{
		auto mode = std::ios::out;
		if(app)
			mode=std::ios::app;
		std::ofstream out(filename, mode);
		out << count << ' ';
		for (auto &data : datas)
			data.writeString(out);
	}
	void readString(std::string filename)
	{
		std::ifstream in(filename);
		in >> count;
		if (!datas.empty()) datas.clear();
		for (int i = 0; i < count; i++)
		{
			T data;
			data.readString(in);
			datas.push_back(data);
		}
	}
};
