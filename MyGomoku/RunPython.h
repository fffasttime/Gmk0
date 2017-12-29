#pragma once
#include "Utils.h"
#include "Python.h"


class PythonInstance
{
private:
	static bool created;

public:
	PythonInstance()
	{
		assert(!created);
		Py_Initialize();
	}
	~PythonInstance()
	{
		Py_Finalize();
	}
	void callScirpt(string filename);
};
