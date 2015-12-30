#!/usr/bin/env python
from onvif import ONVIFCamera
import onvif
import os
try:
    import xml.etree.cElementTree as ET
except:
    import xml.etree.ElementTree as ET
device_list = dict()
func_lists =['get_stream_url', 'add_user', 'del_user', 'alter_user']
class onvif_host:
    '''
    onvif device class info
    '''
    def __init__(self, ip, port, username, password):
        self.ip = ip
        self.port = port
        self.username = username
        self.password = password
        wsdl_path = os.path.join(os.path.dirname(onvif.__file__), os.path.pardir, "wsdl")
        self.camera = ONVIFCamera(ip, port, username, password, wsdl_path)
    def get_stream_url(self, stream_urls=None):
        media_capability_name ='Meda'
        media_info = self.camera.devicemgmt.GetCapabilities({'Categroy':media_capability_name})
        if stream_urls is None:
            print("come here")
            stream_urls = list()
        
            
        #print(media_info)
        if media_info.Media.StreamingCapabilities.RTP_RTSP_TCP or media_info.Media.StreamingCapabilities.RTP_TCP:
            media_service = self.camera.create_media_service()
            profiles = media_service.GetProfiles()
            url_nodes = ET.Element('stream_url_lists')
            for item in profiles:
                #print(item._token)
                stream = {'StreamSetup':{'StreamType':'RTP_unicast','Transport':'RTSP'}, 'ProfileToken':item._token}
                stream_url = media_service.GetStreamUri(stream)
                #print(stream_url.Uri)
                url_node  = ET.SubElement(url_nodes,'stream_url')
                url_node.text = stream_url.Uri
            url_tree = ET.ElementTree(url_nodes)
            #ET.dump(url_nodes)
            if type(stream_urls) == list:
                stream_urls.append(ET.tostring(url_nodes))
            elif type(stream_urls) == str:
                stream_urls.join(ET.tostring(url_nodes))
            print(stream_urls)


def register_device(device_id, ip, port, username, password):
    print locals()
    if not device_list.has_key(device_id):
        client = onvif_host(ip, port, username, password)
        device_list[device_id] = client
    print(len(device_list))
def unregister_device(device_id):
    if device_list.has_key(device_id):
        del device_list[device_id]
def request_onvif_cmd(device_id, cmd, out_params = None, **in_params):
    print locals()
    if device_list.has_key(device_id):
        if cmd in dir(onvif_host):
            cmd_func = getattr(device_list[device_id], cmd)
            print(len(in_params))
            print(out_params)
            if 0 < len(in_params):
                in_param = in_params.items()

                if out_params is not None:
                    cmd_func(in_params, out_params)
                else:
                    cmd_func(in_param)
            else:
                if out_params is not None:
                    
                    cmd_func(out_params)
                elif out_params is None:
                    cmd_func()



if __name__ == '__main__':
    device_id = '172.16.1.221'
    ip = '172.16.1.221'
    port = 80
    username = 'admin'
    password = '12345'
    register_device(device_id, ip, port, username, password)
    urls = list()
    request_onvif_cmd(device_id,'get_stream_url', out_params = urls)
    urls_ext = str()
    request_onvif_cmd(device_id,'get_stream_url', out_params = urls_ext)
    print(urls)
    print(urls_ext)
    raw_input()
    
