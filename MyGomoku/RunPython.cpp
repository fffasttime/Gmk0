#include "RunPython.h"

bool PythonInstance::created = false;

void PythonInstance::callScirpt(string filename)
{
	using std::cout;
	//string chdir_cmd = string("sys.path.append(\"") + filename + "\")";
	//const char* cstr_cmd = chdir_cmd.c_str();
	//PyRun_SimpleString("import sys");
	//PyRun_SimpleString(cstr_cmd);

	// 加载模块
	PyObject* moduleName = Py_BuildValue("s","Estimate"); //模块名，不是文件名
	PyObject* pModule = PyImport_Import(moduleName);
	if (!pModule) // 加载模块失败
	{
		cout << "[ERROR] Python get module failed.\n";
		return;
	}
	cout << "[INFO] Python get module succeed.\n";

	// 加载函数
	PyObject* pv = PyObject_GetAttrString(pModule, "test_add");
	if (!pv || !PyCallable_Check(pv))  // 验证是否加载成功
	{
		cout << "[ERROR] Can't find function (test_add)\n";
		return;
	}
	cout << "[INFO] Get function (test_add) succeed.\n";

	// 设置参数
	PyObject* args = PyTuple_New(2);   // 2个参数
	PyObject* arg1 = Py_BuildValue("i",4);    // 参数一设为4
	PyObject* arg2 = Py_BuildValue("i",3);    // 参数二设为3
	PyTuple_SetItem(args, 0, arg1);
	PyTuple_SetItem(args, 1, arg2);
	
	// 调用函数
	PyObject* pRet = PyObject_CallObject(pv, args);

	// 获取参数
	if (pRet)  // 验证是否调用成功
	{
		int result,r2=0;
		PyArg_ParseTuple(pRet, "(i)", &result);
		cout << "result:" << result<<' '<<r2;
	}
}
