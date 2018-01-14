#include "RunPython.h"
#include "arrayobject.h"

bool PythonInstance::created = false;
PythonInstance* PythonInstance::instance = nullptr;

int PythonInstance::initNumpy()
{
	import_array();
	return 1;//SUCCESS
}

PythonInstance::PythonInstance()
{
	assert(!created);
	instance = this;
	Py_Initialize();
	//if (!initNumpy()) std::cout<<"[ERROR] Fail loading numpy.\n";
//change path
#if 1
	string chdir_cmd = string("sys.path.append('./')");
	const char* cstr_cmd = chdir_cmd.c_str();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString(cstr_cmd);
#endif
	PyRun_SimpleString("import tensorflow as tf");
}

void PythonInstance::loadPackage(string packname)
{
	using std::cout;
	// 加载模块
	PyObject* moduleName = Py_BuildValue("s", packname.c_str()); //模块名，不是文件名
	PyObject* pModule = PyImport_Import(moduleName);
	if (!pModule) // 加载模块失败
	{
		cout << "[ERROR] Python get module failed.\n";
		return;
	}
	packages[packname] = pModule;
	cout << "[INFO] Python get module succeed.\n";
}

void PythonInstance::loadFunction(string packname, string funcname)
{
	using std::cout;
	auto pModule = packages[packname];
	// 加载函数
	PyObject* pv = PyObject_GetAttrString(pModule, funcname.c_str());
	if (!pv || !PyCallable_Check(pv))  // 验证是否加载成功
	{
		cout << "[ERROR] Can't find function " <<funcname <<"\n";
		return;
	}
	functions[funcname] = pv;
	cout << "[INFO] Get function "<< funcname <<" succeed.\n";
}

PyObject *PythonInstance::getFunc(string funcname)
{
	return functions[funcname];
}

void testCall()
{
	double CArrays[3][3] = { { 1.3, 2.4, 5.6 },{ 4.5, 7.8, 8.9 },{ 1.7, 0.4, 0.8 } };
	npy_intp Dims[2] = { 3, 3 };
	PyObject *PyArray = PyArray_SimpleNewFromData(2, Dims, NPY_DOUBLE, CArrays);

	// 设置参数
	PyObject* args = PyTuple_New(2);   // 2个参数
	PyObject* arg1 = Py_BuildValue("i", 4);    // 参数一设为4
	PyObject* arg2 = Py_BuildValue("i", 3);    // 参数二设为3
	auto py = PythonInstance::getInstance();
	auto fun = py->getFunc("test_add");
	PyTuple_SetItem(args, 0, arg1);
	PyTuple_SetItem(args, 1, arg2);

	// 调用函数
	PyObject* pRet = PyObject_CallObject(fun, args);

	// 获取参数
	if (pRet)  // 验证是否调用成功
	{
		int result, r2 = 0;
		PyArg_ParseTuple(pRet, "ii", &result, &r2);
		std::cout << "result:" << result << ' ' << r2;
	}
}
