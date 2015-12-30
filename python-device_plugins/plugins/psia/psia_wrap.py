#!/usr/bin/env python
#-*- coding=utf8 -*-
import requests
import collections
try:
    import xml.etree.cElementTree as ET
except:
    import xml.etree.ElementTree as ET
device_info = collections.namedtuple('device_info', 'device_id ip, port, user, pwd')
device_lists = dict()

class psia_converter():
    """ 转换PISA内容为公司标准XML"""
    def __init__(self, content):
        self.content = content
        ET.register_namespace('', 'urn:psialliance-org')
        self.xml_node = ET.fromstring(self.content)
        print(self.xml_node)
        self.psia_ns = {'root_ns':'urn:psialliance-org'}
    def to_stream_url(self, device):
        print(locals())
        stream_urls = list()
        #print('tag:', self.xml_node.tag)
        #channel_lists = self.xml_node.find('{urn:psialliance-org}StreamingChannelList', self.psia_ns)
        if self.xml_node.tag != '{urn:psialliance-org}StreamingChannelList':
            return ''
        out_streams_xml_node = ET.Element('streamurls')
        for channel in self.xml_node.iter('{urn:psialliance-org}StreamingChannel'):
        #for channel in channel_lists:
            enable_node = channel.find('root_ns:enabled', self.psia_ns)
            video_node = channel.find('root_ns:Video', self.psia_ns)
            channel_num = int(channel.find('root_ns:id', self.psia_ns).text)
            transport_node = channel.find('root_ns:Transport', self.psia_ns)
            b_support_rtsp = False
            for protocol in transport_node.find('root_ns:ControlProtocolList', self.psia_ns):
                protocol_context_node = protocol.find('root_ns:streamingTransport', self.psia_ns)
                if protocol_context_node is not None and protocol_context_node.text == 'RTSP':
                    b_support_rtsp = True
                    break
            if video_node is not None:
                video_enable_node = video_node.find('root_ns:enabled', self.psia_ns)
                if enable_node is not None and enable_node.text and video_node is not None and video_enable_node.text and b_support_rtsp:
                    stream_url = 'rtsp://{0}:{1}/Streaming/Channels/{2}?transportmode={3}&profile=Profile_{4}'.format(device.ip, device.port, channel_num, 'unicast', channel_num)
                    out_stream_xml_node = ET.SubElement(out_streams_xml_node,'stream_url')
                    out_stream_xml_node.text = stream_url
                    stream_urls.append(stream_url)
        print(stream_urls, len(stream_urls))
        out_streams_str = ET.tostring(out_streams_xml_node, encoding='UTF-8', method='xml')
        return out_streams_str

    def to_device_status_xml(self):
        #device_info_node = self.xml_node.find('root_ns:DeviceInfo', self.psia_ns)
        #print('device_info:',device_info_node)
        device_status_node = ET.Element('device_status')
        if self.xml_node.tag != 'DeviceStatus':
            device_status_node.text = 'false'
        else:
            device_status_node.text = 'true'
        device_status_xml = ET.tostring(device_status_node, encoding='UTF-8', method='xml')
        return device_status_xml
    def std_xml(self, xml_type, device = None):
        if hasattr(self, xml_type):
            func = getattr(self, xml_type, None)
            if func is not None:
                if device is not None:
                    return func(device)
                else:
                    return func()
            else:
                return ''

class psia_uri_converter():
    """ URI 转换为PSIA标准URI"""
    def __init__(self, func_name, device):
        #self._psia_uri = self._to_psia_uri(func_name)[0]
        self._device = device
        (self._psia_uri,self._method) = self._to_psia_uri(func_name)
    def psia_uri(self):
        return self._psia_uri
    def method(self):
        return self._method
    def _to_psia_uri(self, func_name):
        if func_name == 'get_stream_url':
            return ('http://{0}:{1}/PSIA/Streaming/channels'.format(self._device.ip, self._device.port),'GET')
        elif func_name == 'get_device_status':
            return  ('http://{0}:{1}/PSIA/System/status'.format(self._device.ip, self._device.port), 'GET')

def register_device(device_id, ip, port, user_name, user_pwd):
    """ 注册设备"""
    global device_lists
    print(locals())
    register_node = ET.Element('register')
    ip_node = ET.SubElement(register_node, 'ip')
    ip_node.text = ip
    session_node = ET.SubElement(register_node, 'session')
    if device_id not in device_lists:
        device_lists[device_id] = device_info(device_id=device_id, ip=ip, port=port, user = user_name, pwd = user_pwd)
    register_xml = ET.tostring(register_node, encoding="UTF-8", method="xml")
    print('out:', register_xml, 'len:', len(register_xml))
    return (register_xml, len(register_xml))
def unregister_device(device_id):
    """ 反注册设备 """
    global device_lists
    if device_id in device_lists:
        device_lists.pop(device_id)
def request(device_id, uri, method):
    global device_lists
    print(locals())
    if device_id not in device_lists:
        return ("", 0)
    else:
        request_auth = (device_lists.get(device_id).user, device_lists.get(device_id).pwd)
        response = requests.request(method, uri, auth=request_auth)
        if response.status_code == 200:
            return response.text
        else:
            print('error code:', response.status_code, 'text:', response.text)
            return ''

def get_stream_url(device_id, channel=None):
    global device_lists
    print(locals())
    if device_id not in device_lists:
        return ("", 0)
    tmp_psia_uri = psia_uri_converter('get_stream_url', device_lists.get(device_id))
    tmp_out_data = request(device_id, tmp_psia_uri.psia_uri(), tmp_psia_uri.method())
    xml_converter = psia_converter(tmp_out_data)
    out_xml = xml_converter.std_xml('to_stream_url', device_lists.get(device_id))
    return (out_xml, len(out_xml))

def get_device_status(device_id):
    global device_lists
    print(locals())
    if device_id not in device_lists:
        return ("", 0)
    tmp_psia_uri = psia_uri_converter('get_device_status', device_lists.get(device_id))
    tmp_out_data = request(device_id, tmp_psia_uri.psia_uri(), tmp_psia_uri.method())
    xml_converter_result = psia_converter(tmp_out_data)
    out_xml = xml_converter_result.std_xml('to_device_status_xml')
    return (out_xml, len(out_xml))
if __name__ == '__main__':
    register_device('111','172.16.1.190',80,'admin','12345')
    #out = request('111', 'http://172.16.1.190:80/PSIA/Streaming/channels', 'GET')
    #print('out:', out, 'len:', len(out), 'type:', type(out))
    out = get_stream_url('111')
    print('out:', out, 'len:', len(out), 'type:', type(out))
    out = get_device_status('111')
    print('out:', out, 'len:', len(out), 'type:', type(out))








