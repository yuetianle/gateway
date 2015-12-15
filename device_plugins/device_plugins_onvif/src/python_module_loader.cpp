#include "stdafx.h"
#include "python_module_loader.h"

python_module_loader::python_module_loader()
{
}

python_module_loader::~python_module_loader()
{
}

int python_module_loader::init()
{
#if _WIN32
	char pySearchPath[] = "onvif\\python27";
	Py_SetPythonHome(pySearchPath);
#endif
	cout << Py_GetPythonHome();
	Py_Initialize();

	if (Py_IsInitialized())
	{
		vvlog_i("Py_IsInitialized success");
#ifdef _DEBUG
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("print(sys.path)");
#endif
		return v_OK;
	}
	else
	{
		vvlog_i("Py_IsInitialized fail");
		return v_ERR_Not_Supported;
	}
}

int python_module_loader::uninit()
{
	if (Py_IsInitialized())
	{
		Py_Finalize();
	}
	return v_OK;
}

PyObject* python_module_loader::load_module(const string &module_name)
{
	PyObject *pname = nullptr;
	pname = PyString_FromString(module_name.c_str());
	PyObject *pmodule = nullptr;
	if (pname)
	{
		pmodule = PyImport_Import(pname);
		if (!pmodule)
			vvlog_e("PyImport_Import faile, name:" << module_name.c_str());
	}
	return pmodule;
}

PyObject* python_module_loader::get_func(PyObject *p, const string &func_name)
{
	PyObject *func_obj = nullptr;
	if (p && !func_name.empty())
	{
		PyObject *pdict = PyModule_GetDict(p);
		if (pdict)
		{
			func_obj = PyDict_GetItemString(pdict, func_name.c_str());
		}
		if (!func_obj || !PyCallable_Check(func_obj))
		{
			vvlog_e("PyCallable_Check fail func:" << func_name << "not exist or not callable.");
			return nullptr;
		}
	}
	return func_obj;
}

PyObject* python_module_loader::call_func(PyObject *func , PyObject *args, PyObject *argstwo /* = nullptr */, PyObject *argsthree /* = nullptr */)
{
	if (func && args)
	{
		return PyEval_CallObject(func, args);
	}
	if (func && args && argstwo && argsthree)
	{
		return PyEval_CallObjectWithKeywords(args, argstwo, argsthree);
	}
	return nullptr;
}


/*int python_module_loader::convert_C_to_Python_Params(const string &params, const string &func_name, PyObject *out_args)
{
	return v_OK;
}
*/
