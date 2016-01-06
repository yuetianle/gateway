#!/usr/bin/env python
# -*- coding:utf-8 -*-

#import onvif_wrap
import inspect
import time
from urlobject import URLObject

from onvif import ONVIFCamera
import onvif
import os
import sys
import collections
import vonvif as this
try:
    import xml.etree.cElementTree as ET
except:
    import xml.etree.ElementTree as ET


device_list = dict()

Session = collections.namedtuple('Session','client ip port user pwd')
def register_device(device_id, ip, port, user_name, user_pwd):
    print locals()
    global device_list
    register_node = ET.Element('register')
    ip_node = ET.SubElement(register_node, 'ip')
    ip_node.text = ip
    session_node = ET.SubElement(register_node, 'session')
    if device_id not in device_list:
        if sys.platform == 'win32':
            wsdl_path = os.path.join(os.path.dirname(onvif.__file__), os.path.pardir, "wsdl")
            print('wsdl:', wsdl_path)
            client = ONVIFCamera(ip, port, user_name, user_pwd, wsdl_path)
            session = Session(client=client, ip=ip, port=port, user=user_name, pwd=user_pwd)
            device_list[device_id] = session
        else:
            client = ONVIFCamera(ip, port, user_name, user_pwd)
            session = Session(client=client, ip=ip, port=port, user=user_name, pwd=user_pwd)
            device_list[device_id] = session
        session_node.text = ip
    else:
        session_node.text = str(device_list.get(device_id).ip)
    session_xml = ET.tostring(register_node, encoding='UTF-8', method='xml')
    #print('session_xml:', session_xml, 'type', type(session_xml), 'len:', len(session_xml), 'type:', type(len(session_xml)))
    return (session_xml, len(session_xml))

def un_register_device(device_id):
    print(locals())
    global device_list
    if device_id in device_list:
        device_list.pop(device_id)

def get_stream_url(device_id, channel=None):
    print(locals())
    global device_list
    urls = ET.Element('stream_url_lists')
    if device_id not in device_list:
        urls_xml = ET.tostring(urls, encoding='UTF-8', method='xml')
        return (urls_xml, len(urls_xml))
    else:
        session = device_list.get(device_id)
        media_capability_name ='Meda'
        media_info = session.client.devicemgmt.GetCapabilities({'Categroy':media_capability_name})
        if media_info.Media.StreamingCapabilities.RTP_RTSP_TCP or media_info.Media.StreamingCapabilities.RTP_TCP:
            media_service = session.client.create_media_service()
            profiles = media_service.GetProfiles()
            for item in profiles:
                stream = {'StreamSetup':{'StreamType':'RTP_unicast','Transport':'RTSP'}, 'ProfileToken':item._token}
                stream_url = media_service.GetStreamUri(stream)
                url_node  = ET.SubElement(urls,'stream_url')
                url_node.text = stream_url.Uri
    urls_xml = ET.tostring(urls, encoding='UTF-8', method='xml')
    return (urls_xml, len(urls_xml))

def get_device_status(device_id, channel=None):
    global device_list
    print(locals())
    device_status = ET.Element('device_status')
    if device_id not in device_list:
        device_status.text = 'false'
    else:
        time = device_list.get(device_id).client.devicemgmt.GetSystemDateAndTime()
        if time is not None:
            device_status.text = 'true'
        else:
            device_status.text = 'false'
    status_xml = ET.tostring(device_status, encoding='UTF-8', method='xml')
    return (status_xml, len(status_xml))

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
    #func_lists = dir(onvif_wrap)
    func_lists = dir(this)
    parser = uri_parser(uri)
    parser.add_func_param({'device_id':device_id})
    print('begin device_id:', device_id, 'uri:', uri, 'params:', params, 'func_param:', parser.func_params('func'))
    func_name = parser.func_name('func')
    if func_name in func_lists:
        #cmd_func = getattr(onvif_wrap, func_name)
        cmd_func = getattr(this, func_name)
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
    print('vonvif:', dir())
    #cmd_func = getattr(this, 'register_device')
    #print('cmd_func:', cmd_func)
    out_data = request_cmd('111', "http://admin:12345@172.16.1.190:8000/device/meida?func=register_device", '')
    print('receive:', out_data)
    out_data = request_cmd('111', "http://172.16.1.190:8000/device/meida?func=get_stream_url&channel=1", '')
    print('receive:', out_data)
    while 1:
        out_data = request_cmd('111', "http://172.16.1.190:8000/device/meida?func=get_device_status", '')
        print('receive:', out_data)
        time.sleep(5)
