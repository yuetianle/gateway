#!/usr/bin/env python
#-*- coding=utf-8 -*-

import inspect
from urlobject import URLObject
import psia_wrap

class uri_parser():
    """uri parser"""
    def __init__(self, uri):
        """init function"""
        self._uri = URLObject(uri)
        self._params = self._uri.query.dict
        self._func_name = ''
        self._psia_uri = self.__psia_uri__()
        #print('psia_uri:', self._psia_uri)
    def __psia_uri__(self):
        if self.func_name('func') == 'get_stream_url':
            return 'http://{0}:{1}/PSIA/Streaming/channels'.format(self.ip(), self.port())
        else:
            return ''
    def user_name(self):
        return self._uri.username
    def password(self):
        return self._uri.password
    def ip(self):
        return self._uri.hostname
    def port(self):
        return self._uri.port
    def add_func_param(self, param):
        self._params.update(param)
    def func_name(self, name):
        if name in self._params:
            self._func_name = self._params[name]
            print('func_name:', self._func_name)
            return self._func_name
        else:
            return ''
    def func_params(self, name):
        if name in self._params and self._params[name] == 'register_device':
            self.add_func_param({'ip':self._uri.hostname})
            self.add_func_param({'port':self._uri.port})
            self.add_func_param({'user_name':self._uri.username})
            self.add_func_param({'user_pwd':self._uri.password})
        #PSIA params
        #if (name in self._params and self._params[name] != 'register_device'and self._params[name] != 'unreigster_device'):
            #self.add_func_param({'uri':self._psia_uri})
            #if 0 <= self._func_name.find('get'):
                #self.add_func_param({'method':'GET'})
        if name in self._params:
            self._params.pop(name)
        return self._params


def request_cmd(device_id, uri, params):
    """device cmd"""
    func_lists = dir(psia_wrap)

    parser = uri_parser(uri)
    parser.add_func_param({'device_id':device_id})

    func_name = parser.func_name('func')
    #if func_name != 'register_device' and func_name != 'unregister_device':
        #func_name = 'request'
    print('list:',func_lists, 'name:', func_name)
    if func_name in func_lists:
        cmd_func = getattr(psia_wrap, func_name)
        cmd_params = parser.func_params('func')
        print('begin device_id:', device_id, 'uri:', uri, 'params:', params, 'cmdname:', func_name, 'cmd:', cmd_func, 'func_param:', parser.func_params('func'))
        params_lists = []
        need_args = inspect.getargspec(cmd_func).args
        for call_args in need_args:
            if cmd_params.has_key(call_args):
                params_lists.append(cmd_params.get(call_args))
        out_data = cmd_func(**cmd_params)
        print("end out_data:", out_data, "type:", type(out_data))
        return out_data

if __name__ == '__main__':
    out = request_cmd('172.16.1.190', 'http://admin:12345@172.16.1.190:80/device/meida?func=register_device', '')
    #print('out:', out, 'len:', len(out), 'type:',type(out))
    out = request_cmd('172.16.1.190', 'http://172.16.1.190:80/device/meida?func=get_stream_url', '')
    #print('out:', out, 'len:', len(out), 'type:',type(out))