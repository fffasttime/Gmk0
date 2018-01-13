#include "Evaluation.h"
#include "RunPython.h"
#include "arrayobject.h"

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
	//py->initNumpy();
	_import_array();
	auto fun = py->getFunc("nptest");

	auto input = RawInput(board);
	npy_intp Dims[2] = { 2, BLSIZE };
	PyObject *PyArray = PyArray_SimpleNewFromData(2, Dims, NPY_FLOAT, input.feature);

	auto args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, PyArray);

	auto pRet = PyObject_CallObject(fun, args);
	PyArrayObject *p1, *p2;
	PyArg_ParseTuple(pRet, "OO", &p1, &p2);
	using std::cout;

	PyArrayIterObject *pit1, *pit2;
	pit1 = (PyArrayIterObject *)PyArray_IterNew((PyObject *)p1);
	pit2 = (PyArrayIterObject *)PyArray_IterNew((PyObject *)p2);
	for (; pit1->index < pit1->size; )
	{
		cout << *(float *)(pit1->dataptr);
		PyArray_ITER_NEXT(pit1);
	}
	cout << '\n';
	for (; pit2->index < pit2->size; )
	{
		cout << *(float *)(pit2->dataptr);
		PyArray_ITER_NEXT(pit2);
	}
}
