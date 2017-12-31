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

	PythonInstance()
	{
		assert(!created);
		instance = this;
		Py_Initialize();
	}
	~PythonInstance()
	{
		Py_Finalize();
	}
	void loadPackage(string packname);
	void loadFunction(string packname, string funcname);
	PyObject *getFunc(string funcname);
};
