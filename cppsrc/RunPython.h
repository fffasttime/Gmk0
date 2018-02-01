#pragma once
#include "Utils.h"
#include "Python.h"
#include <map>

class PythonInstance
{
private:
	static bool created;
	static PythonInstance* instance;

	std::map<string, PyObject *> packages;
	std::map<string, PyObject *> functions;

public:
	static PythonInstance* getInstance()
	{
		ASSERT(instance != nullptr);
		return instance;
	}

	PythonInstance();
	~PythonInstance()
	{
		Py_Finalize();
	}
	int initNumpy();
	void loadPackage(string packname);
	void loadFunction(string packname, string funcname);
	PyObject *getFunc(string funcname);
};

void testCall();