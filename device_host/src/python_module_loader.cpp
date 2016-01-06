#include "stdafx.h"
#include "python_module_loader.h"

data_cache_ex<string, PyObject*> python_module_loader::s_module_list;
python_module_loader::python_module_loader()
{
}

python_module_loader::~python_module_loader()
{
}

int python_module_loader::init()
{
#if _WIN32
	char pySearchPath[] = "python27";
	Py_SetPythonHome(pySearchPath);
#endif
	cout << Py_GetPythonHome();

	if (!Py_IsInitialized())
	{
		Py_Initialize();
		if (Py_IsInitialized())
		{
			vvlog_i("Py_IsInitialized success");
#ifdef _DEBUG
			PyRun_SimpleString("import sys");
			PyRun_SimpleString("print(sys.path)");
#endif
		}
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
	if (s_module_list.find(module_name, pmodule))
	{
		return pmodule;
	}
	if (pname)
	{
		try
		{
			pmodule = PyImport_Import(pname);
			//pmodule = PyImport_ImportModuleNoBlock(module_name.c_str());
			if (!pmodule)
			{
				PyErr_Print();
			}
			s_module_list.insert(module_name, pmodule);
		}
		catch (exception* e)
		{
			cout << "exception:" << e->what() << endl;
		}
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
		else
		{
			vvlog_i("call func"<< func_obj << "name:" << PyEval_GetFuncName(func_obj) << "desc:" << PyEval_GetFuncDesc(func_obj));
		}
	}
	return func_obj;
}

//PyObject* python_module_loader::call_func(PyObject *func, PyObject *args, PyObject *argstwo /* = nullptr */, PyObject *argsthree /* = nullptr */)
//{
//	if (func && args)
//	{
//		return PyEval_CallObject(func, args);
//	}
//	if (func && args && argstwo && argsthree)
//	{
//		return PyEval_CallObjectWithKeywords(args, argstwo, argsthree);
//	}
//	return nullptr;
//}

PyObject* python_module_loader::call_func(PyObject *func_obj, PyObject *func_args)
{
	if (func_obj)
	{
		PyObject *py_ret = nullptr;
		if (func_args)
		{
			try
			{
				//py_ret = PyObject_CallObject(func_obj, func_args);
				//PyObject_Print(func_obj, stdout, 0);
				//PyErr_SetString(PyExc_Exception, "type error");
				py_ret = PyEval_CallObject(func_obj, func_args);
				/*if (PyErr_Occurred())
				{
					PyErr_SetString(PyExc_Exception, "type error");
				}*/
			}
			catch (exception* e)
			{
				vvlog_e("PyObject_CallObject exception");
			}
		}
		else
		{
			//py_ret = PyObject_CallObject(func_obj, NULL);
			py_ret = PyEval_CallObject(func_obj, NULL);
		}
		if (!py_ret)
		{
			PyErr_Print();
		}
		return py_ret;
	}
	else
	{
		return nullptr;
	}
}

/*int python_module_loader::convert_C_to_Python_Params(const string &params, const string &func_name, PyObject *out_args)
{
return v_OK;
}
*/
