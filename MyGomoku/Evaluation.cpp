#include "Evaluation.h"
#include "RunPython.h"
#include "arrayobject.h"
#include <Windows.h>
int initNumpy()
{
	import_array();
	return 1;//SUCCESS
}

RawInput::RawInput(Board &board)
{
	for (int i = 0; i < BLSIZE; i++)
		if (board[i] == C_B)
		{
			feature[0][i] = 1; 
			feature[1][i] = 0;
		}
		else if (board[i]==C_W)
		{
			feature[0][i] = 0; 
			feature[1][i] = 1;
		}
		else
		{
			feature[0][i] = 0; 
			feature[1][i] = 0;
		}
}

void getEvaluation(Board &board, RawOutput &output)
{
	auto py = PythonInstance::getInstance();
	auto fun = py->getFunc("nptest");

	auto input = RawInput(board);
	npy_intp Dims[2] = { 2, BLSIZE };
	PyObject *PyArray = PyArray_SimpleNewFromData(2, Dims, NPY_FLOAT, input.feature);

	auto args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, PyArray);

	auto pRet = PyObject_CallObject(fun, args);
	Sleep(1000);
	PyArrayObject *p1, *p2;
	PyArg_ParseTuple(pRet, "OO", &p1, &p2);
	using std::cout;
	
	PyArrayIterObject *pit1, *pit2;
	pit1 = (PyArrayIterObject *)PyArray_IterNew((PyObject *)p1);
	pit2 = (PyArrayIterObject *)PyArray_IterNew((PyObject *)p2);
	for (int i=0; pit1->index < pit1->size; i++)
	{
		output.p[i] = *(float *)pit1->dataptr;
		PyArray_ITER_NEXT(pit1);
	}
	for (; pit2->index < pit2->size; )
	{
		output.v = *(float *)pit1->dataptr;
		PyArray_ITER_NEXT(pit2);
	}
	Py_DECREF(args);
	Py_DECREF(p1);
	Py_DECREF(p2);
	Py_DECREF(pRet);
}
