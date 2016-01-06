#ifndef _PYTHON_MODULE_LOADER_H_
#define _PYTHON_MODULE_LOADER_H_
#include "python.h"
#include "data_cache_ex.h"
//#include <boost\python.hpp>
#pragma once
class python_module_loader
{
public:
	python_module_loader();
	~python_module_loader();
public:
	static int init();
	static int uninit();
	/*
	 * @brief 装载模块
	 */
	static data_cache_ex<string, PyObject*> s_module_list;
	PyObject* load_module(const string &module_name);
	/*
	 * @brief 获取函数接口
	 *
	 */
	PyObject* get_func(PyObject *p, const string &func_name);
	/*
	 * @brief 调用函数接口
	 *
	 */
	PyObject* call_func(PyObject *func_obj, PyObject *func_args);

	//PyObject* call_func(PyObject *func , PyObject *args, PyObject *argstwo = nullptr, PyObject *argsthree = nullptr);
	/*
	 * @brief 将C++参数转换为Python参数
	 *
	 */
	//int convert_C_to_Python_Params(const string &uri, const string &func_name, PyObject *out_args);
};
#endif

