#!/usr/bin/env python
# -*- coding:utf-8 -*-
from onvif import ONVIFCamera
import onvif
import os
import sys
import collections
try:
    import xml.etree.cElementTree as ET
except:
    import xml.etree.ElementTree as ET

device_list = dict()

Session = collections.namedtuple('Session','client ip port user pwd')
def register_device(device_id, ip, port, username, password):
    print locals()
    global device_list
    register_node = ET.Element('register')
    ip_node = ET.SubElement(register_node, 'ip')
    ip_node.text = ip
    session_node = ET.SubElement(register_node, 'session')
    if device_id not in device_list:
        if sys.platform == 'win32':
            wsdl_path = os.path.join(os.path.dirname(onvif.__file__), os.path.pardir, "wsdl")
            client = ONVIFCamera(ip, port, username, password, wsdl_path)
            session = Session(client=client, ip=ip, port=port, user=username, pwd=password)
            device_list[device_id] = session
        else:
            client = ONVIFCamera(ip, port, username, password)
            session = Session(client=client, ip=ip, port=port, user=username, pwd=password)
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


if __name__ == '__main__':
    out = register_device('172.16.1.190', '172.16.1.190', 80, 'admin', '12345')
    print('out:', out, 'type:', type(out))
    out = get_stream_url('172.16.1.190')
    print('out:', out, 'type:', type(out))
    out = get_device_status('172.16.1.190')
    print('out:', out, 'type:', type(out))


