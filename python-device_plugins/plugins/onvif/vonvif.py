#！/usr/bin/env python
# -*- coding:utf-8 -*-

import onvif_wrap
import inspect
import time
from urlobject import URLObject

class uri_parser():
    """uri parser"""
    def __init__(self, uri):
        """init function"""
        self.uri = URLObject(uri)
        self.params = self.uri.query.dict
    def user_name(self):
        return self.uri.username
    def password(self):
        return self.uri.password
    def ip(self):
        return self.uri.hostname
    def port(self):
        return self.uri.port
    def add_func_param(self, param):
        self.params.update(param)
    def func_name(self, name):
        query = self.uri.query.dict

        if query.has_key(name):
            return query[name]
        else:
            return ''
    def func_params(self, name):
        query = self.uri.query.dict
        if query[name] == 'register_device':
            self.add_func_param({'ip':self.uri.hostname})
            self.add_func_param({'port':self.uri.port})
            self.add_func_param({'user_name':self.uri.username})
            self.add_func_param({'user_pwd':self.uri.password})
        if self.params.has_key(name):
            self.params.pop(name)
        print('params:', self.params)
        return self.params

def request_cmd(device_id, uri, params):
    """device cmd"""
    func_lists = dir(onvif_wrap)
    parser = uri_parser(uri)
    parser.add_func_param({'device_id':device_id})
    print('begin device_id:', device_id, 'uri:', uri, 'params:', params, 'func_param:', parser.func_params('func'))
    func_name = parser.func_name('func')
    if func_name in func_lists:
        cmd_func = getattr(onvif_wrap, func_name)
        cmd_params = parser.func_params('func')
        params_lists = []
        need_args = inspect.getargspec(cmd_func).args
        for call_args in need_args:
            if cmd_params.has_key(call_args):
                params_lists.append(cmd_params.get(call_args))
        out_data = cmd_func(**cmd_params)
        print("end out_data:", out_data, "type:", type(out_data))
        return out_data

if __name__ == '__main__':
    out_data = request_cmd('111', "http://admin:12345@172.16.1.190:8000/device/meida?func=register_device", '')
    print('receive:', out_data)
    out_data = request_cmd('111', "http://172.16.1.190:8000/device/meida?func=get_stream_url&channel=1", '')
    print('receive:', out_data)
    while 1:
        out_data = request_cmd('111', "http://172.16.1.190:8000/device/meida?func=get_device_status", '')
        print('receive:', out_data)
        time.sleep(5)
