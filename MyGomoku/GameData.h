#pragma once

#include "Utils.h"
#include <vector>

struct EposideData
{
	typedef unsigned char ubyte;
	int stepcount;
	std::vector<int> moves;
	int z;

	EposideData() = default;
	EposideData(const std::vector<int> &_moves, int _z) :moves(_moves), z(_z)
	{
		stepcount = moves.size();
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
		std::string s; ss >> s;
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
	typedef float StepPolicy[POLICY_MAX_EXPANDS];
	int stepcount;
	std::vector<int> moves;
	std::vector<StepPolicy> policy;
	int z;

	EposideTrainingData() = default;
	EposideTrainingData(const std::vector<int> &_moves, const std::vector<StepPolicy> &_policy, int _z) :moves(_moves), policy(_policy), z(_z)
	{
		stepcount = moves.size();
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
		ss << stepcount << ' ' << z;
		for (int i=0;i<stepcount;i++)
		{
			ss << ' '<< moves[i];
			for (int j = 0; j < POLICY_MAX_EXPANDS; j++)
				ss << ' '<< policy[i][j];
		}
		std::string s; ss >> s;
		return s;
	}
	void writeByte(std::ofstream &out)
	{
		out << (ubyte)stepcount << (ubyte)z;
		for (int i = 0; i < stepcount; i++)
		{
			out << (ubyte)moves[i]; 
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
	int count;
	std::vector<T> datas;
	void writeByte(std::ofstream &out)
	{
		out << (ubyte)count;
		for (auto &data : datas)
			data.writeByte(out);
	}
	void readByte(std::ifstream &in)
	{
		ubyte u1;
		in >> u1;
		count = u1;
		for (int i = 0; i < count; i++)
		{
			T data;
			data.readByte(in);
			datas.push_back(data);
		}
	}
	void readString(std::ifstream &in)
	{
		in >> count;
		for (int i = 0; i < count; i++)
		{
			T data;
			data.readString(in);
			datas.push_back(data);
		}
	}
	void writeString(std::ofstream &out)
	{
		out << count << ' ';
		for (auto &data : datas)
			data.writeString(out);
	}
};
