// boostpython.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
char const* greet()
{
	return "hello, world";
}

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(hello)
{
	def("greet", greet);
}
