#!/usr/bin/env python
# -*- coding: utf-8 -*-
from ctypes import *
import sys
import os
import collections
from threading import RLock
import test_logger
import threading
import time

try:
    import xml.etree.cElementTree as ET
except:
    import xml.etree.ElementTree as ET
if sys.platform == 'win32':
    #DllHandle = windll
    DllHandle = None
else:
    DllHandle = cdll

dll_mutex = RLock()
Session = collections.namedtuple('Session', 'session_id ip port user pwd')

device_mutex = RLock()
device_lists = dict()

xml_mutex = RLock()
device_xml_node = ET.Element('devices')
class device_info_v30(Structure):
    _fields_ = [
        ('sSerialNumber', c_ubyte*48),
        ('byAlarmInPortNum', c_ubyte),
        ('byAlarmOutPortNum', c_ubyte),
        ('byDiskNum', c_ubyte),
        ('byDVRType', c_ubyte),
        ('byChanNum', c_ubyte),
        ('byStartChan', c_ubyte),
        ('byAudioChanNum', c_ubyte),
        ('byIPChanNum', c_ubyte),
        ('byZeroChanNum', c_ubyte),
        ('byMainProto', c_ubyte),
        ('bySubProto', c_ubyte),
        ('bySupport', c_ubyte),
        ('bySupport1', c_ubyte),
        ('bySupport2', c_ubyte),
        ('wDevType', c_ushort),
        ('bySupport3', c_ubyte),
        ('byMultiStreamProto', c_ubyte),
        ('byStartDChan', c_ubyte),
        ('byStartDTalkChan', c_ubyte),
        ('byHighDChanNum', c_ubyte),
        ('bySupport4', c_ubyte),
        ('byLanguageType', c_ubyte),
        ('byVoiceInChanNum', c_ubyte),
        ('byStartVoiceInChanNo', c_ubyte),
        ('byRes3', c_ubyte*2),
        ('byMirrorChanNum', c_ubyte),
        ('wStartMirrorChanNo', c_ubyte),
        ('byRes2', c_ubyte*2)];

loginresult= CFUNCTYPE(c_void_p, c_long, c_ulong, POINTER(device_info_v30), c_void_p)
#loginresult= WINFUNCTYPE(c_void_p, c_long, c_ulong, POINTER(device_info_v30), c_void_p)

class login_info(Structure):
    _fields_ = [('s_DeviceAddress', c_char*129),
                ('byRes1', c_ubyte),
                ('wPort', c_ushort),
                ('sUserName', c_char*64),
                ('sPassword', c_char*64),
                #('cbLoginResult', CFUNCTYPE(c_void_p, c_long, c_longlong, pointer(device_info_v30), c_void_p)),
                ('cbLoginResult', loginresult),
                ('pUser', c_void_p),
                ('bUseAsynLogin', c_bool),
                ('byRes2', c_char*128)];
    def __str__(self):
        return '{0}:{1}:{2}:{3}'.format(self.s_DeviceAddress, self.wPort, self.sUserName, self.sPassword)
class deviceinfo_v40(Structure):
    _fields_ = [('strDeviceV30', device_info_v30),
                ('bySupportLock', c_ubyte),
                ('byRetryLoginTime', c_ubyte),
                ('byPasswordLevel', c_ubyte),
                ('byRes1', c_ubyte),
                ('dwSurplusLockTime', c_ulong),
                ('byRes2', c_ubyte*256)];
    def __str__(self):
        return '{0}{1}'.format(self.strDeviceV30.byIPChanNum, self.byRetryLoginTime)
class hik_time(Structure):
    _fields_ = [('dwYear',c_ulong),
                ('dwMonth',c_ulong),
                ('dwDay',c_ulong),
                ('dwHour',c_ulong),
                ('dwMinute',c_ulong),
                ('dwSecond',c_ulong),];
    def __str__(self):
        return '{0}-{1}-{2} {3}:{4}:{5}'.format(self.dwYear, self.dwMonth, self.dwDay, self.dwHour, self.dwMinute, self.dwSecond)

class rtspcfg(Structure):
    _fields_ = [('dwSize', c_ulong),
                ('wPort', c_ushort),
                ('byReserve', c_ubyte*54)];
    def __str__(self):
        return '{0}:{1}'.format(self.dwSize, self.wPort)
def load_dll(dll_name):
    global DllHandle
    global dll_mutex
    if DllHandle is not None:
        test_logger.logger.debug("init success")
        return DllHandle
    else:
        if sys.platform == 'win32':
            other_path = os.path.abspath(os.path.join(os.path.dirname(__file__),'HCNetSDKCom'))
            other_lib = os.path.abspath(os.path.join(os.path.dirname(__file__),'HCCore'))
            sys.path.append(other_path)
            other_dll_handle = WinDLL(other_lib)
            dll_handle = WinDLL(dll_name)
            print('dllname:', dll_name, 'otherlib:', other_dll_handle, 'syspath:', sys.path)
        else:
            dll_handle = CDLL(dll_name)

        dll_mutex.acquire()
        b_init = dll_handle.NET_DVR_Init()
        dll_mutex.release()
        if b_init:
            test_logger.logger.debug("init success")
            return dll_handle
        else:
            test_logger.logger.debug("init fail")
            return None
def register_device(device_id, ip, port, user_name, user_pwd):
    """ register a hikvision device"""
    global DllHandle
    global device_lists
    global device_mutex
    global device_xml_node
    global xml_mutex
    global dll_mutex
    if sys.platform == 'win32':
        dll_path = os.path.abspath(os.path.join(os.path.dirname(__file__),'HCNetSDK'))
        print('handle before:', DllHandle)
        DllHandle = load_dll(dll_path)
        print('handle after:', DllHandle)
    register_node = ET.Element('register')
    ip_node = ET.SubElement(register_node, 'ip')
    ip_node.text = ip
    session_node = ET.SubElement(register_node, 'session')
    device_mutex.acquire()
    if device_id not in device_lists:
        hik_login_info = login_info()
        hik_device_info = deviceinfo_v40()
        memset(addressof(hik_device_info),0, sizeof(deviceinfo_v40))
        memset(addressof(hik_login_info), 0, sizeof(login_info))
        hik_login_info.s_DeviceAddress = ip
        hik_login_info.wPort = port
        hik_login_info.sUserName = user_name
        hik_login_info.sPassword = user_pwd
        dll_mutex.acquire()
        login_id = DllHandle.NET_DVR_Login_V40(pointer(hik_login_info), pointer(hik_device_info))
        DllHandle.NET_DVR_SetConnectTime(5000,3)
        DllHandle.NET_DVR_SetReconnect(5000,1)
        dll_mutex.release()
        xml_mutex.acquire()
        device_node = ET.SubElement(device_xml_node, 'device')
        device_node.set('name', device_id)
        device_node.set('code', device_id)
        device_node.set('manfacture', 'hikvision')
        device_node.set('ip', ip)
        device_node.set('port', str(port))
        device_node.set('parent_id', '')
        tree = ET.ElementTree(device_xml_node)
        tree.write('device_lists.xml', encoding="UTF-8")
        xml_mutex.release()
        login_session = Session(session_id=login_id, ip=ip, port=port, user=user_name, pwd=user_pwd)
        device_lists[device_id] = login_session
        session_node.text = str(login_id)
        device_mutex.release()
    else:
        login_session = device_lists.get(device_id)
        session_node.text = str(login_session.session_id)
        device_mutex.release()
    session_xml = ET.tostring(register_node, encoding="UTF-8", method="xml")
    #print('device_lists:', device_lists)
    print('session_xml:', session_xml, 'type', type(session_xml), 'len:', len(session_xml), 'type:', type(len(session_xml)))
    return (session_xml, len(session_xml))
def get_stream_url(device_id, channel):
    global device_lists
    global DllHandle
    global dll_mutex
    test_logger.logger.debug('get_stream_url start')
    #print('stream_url: device_lists:', device_lists, 'dllhandle:', DllHandle)
    if device_id not in device_lists:
        return ('', 0)
    login_session = device_lists.get(device_id)
    encode_node = ET.Element('AudioVideoCompressInfo')
    channel_node = ET.SubElement(encode_node, 'VideoChannelNumber')
    channel_node.text = str(channel)
    #print("channel:", channel)
    in_xml = ET.tostring(encode_node, encoding="UTF-8", method="xml")
    in_xml_len = c_ulong(len(in_xml))
    #print("inxml:", in_xml, "len:", in_xml_len)
    device_encode_ability_v20 = c_ulong(0x008)
    out_data = create_string_buffer(1024*10)
    out_data_len = c_ulong(1024*10)
    #dll_mutex.acquire()
    ret = DllHandle.NET_DVR_GetDeviceAbility(login_session.session_id, device_encode_ability_v20, cast(in_xml, c_char_p), in_xml_len, out_data, out_data_len)
    hik_rtsp = rtspcfg()
    DllHandle.NET_DVR_GetRtspConfig(login_session.session_id, 0, byref(hik_rtsp), sizeof(rtspcfg))
    #dll_mutex.release()
    stream_urls = list()
    if ret:
        root_node = ET.fromstring(out_data.value)
        #test_logger.logger.debug(root_node)
        channel_lists = root_node.find('./VideoCompressInfo/ChannelList')
        for main_item in channel_lists.iter('ChannelEntry'):
            main_chan = main_item.find('MainChannel')
            channel_index = main_item.find('ChannelNumber')
            #test_logger.logger.debug(channel_index.text)
            if main_chan:
                url_main = 'rtsp://' + str(login_session.ip) + ':' + str(hik_rtsp.wPort) + '/h264/ch' + channel_index.text + '/main/av_stream'
                stream_urls.append(url_main)
            for sub_item in main_item.iter('SubChannelList'):
                url_sub = 'rtsp://' + str(login_session.ip) + ':' + str(hik_rtsp.wPort) + '/h264/ch' + channel_index.text + '/sub/av_stream'
                stream_urls.append(url_sub)
        test_logger.logger.debug('get_stream_url success urls:%s', str(stream_urls))
    else:
        test_logger.logger.debug('get_stream_url fail get xml fail.')
        test_logger.logger.debug("error")
    #test_logger.logger.debug(stream_urls)
    urls = ET.Element('stream_url_lists')
    for item in stream_urls:
        url = ET.SubElement(urls,'stream_url')
        url.text = item
    urls_xml = ET.tostring(urls, encoding='UTF-8', method='xml')
    #urls_xml = ET.tostring(urls, encoding='us-ascii', method='xml')
    #urls_xml = ET.tostring(urls)
    print('return:', urls_xml, 'type:', type(urls_xml), 'len:', len(urls_xml), 'type:',type(len(urls_xml)))
    return (urls_xml, len(urls_xml))
def get_device_status(device_id, channel=None):
    """ get device status"""
    global device_lists
    global DllHandle
    global dll_mutex
    #print(locals())
    #print('status: device_lists:', device_lists, 'dllhandle:', DllHandle)
    test_logger.logger.debug('get_device_status start')
    if device_id not in device_lists:
        return ('', 0)
    if channel is None:
        channel = c_ulong(0xffffffff)
    login_session = device_lists.get(device_id)
    tmp_time = hik_time()
    ret = c_ulong()
    try:
        #dll_mutex.acquire()
    	tmp_ret = DllHandle.NET_DVR_GetDVRConfig(login_session.session_id, 118, channel, pointer(tmp_time), sizeof(hik_time),pointer(ret))
        #dll_mutex.release()
    	device_status = ET.Element('device_status')
        device_status.set('ip', login_session.ip)
        device_status.set('port', str(login_session.port))
    	if tmp_ret:
      	  device_status.text = 'ture'
    	else:
      	  device_status.text = 'false'
    	status_xml = ET.tostring(device_status, encoding='UTF-8', method='xml')
    	#print('return:', status_xml, 'types:', type(status_xml), 'len:', len(status_xml), 'type:', type(len(status_xml)))
        test_logger.logger.debug('get_device_status success statusxml:%s', status_xml)
    	return (status_xml, len(status_xml))
    except:
        test_logger.logger.debug('get_device_status exception statusxml:%s', status_xml)
    	print('exception')
        return ('', 0)
def unregister_device(device_id):
    """ unregister a hikvision device"""


if __name__ == '__main__':
    if sys.platform == 'win32':
        #handle = load_dll('HCNetSDK')
        register_device('172.16.1.190','172.16.1.190', 8000, 'admin', '12345')
        register_device('172.16.1.191','172.16.1.191', 8000, 'admin', '12345')
        get_stream_url('172.16.1.190', 0)
        while 1:
            get_device_status('172.16.1.190')
            time.sleep(5)

    #handle= WinDLL('HCNetSDK')
    #b_init = handle.NET_DVR_Init()

    #test_logger.logger.debug(b_init)
    #hik_login_info = login_info()
    #hik_device_info = deviceinfo_v40()
    #memset(addressof(hik_device_info),0, sizeof(deviceinfo_v40))
    #test_logger.logger.debug('devicesize:',sizeof(deviceinfo_v40))
    #test_logger.logger.debug('login:',sizeof(login_info))
    #memset(addressof(hik_login_info), 0, sizeof(login_info))
    #hik_login_info.s_DeviceAddress = '172.16.1.190'
    #hik_login_info.wPort = 8000
    #hik_login_info.sUserName = 'admin'
    #hik_login_info.sPassword = '12345'
    #test_logger.logger.debug(hik_login_info)
    ##hik_login_info.pUser = 0
    #test_logger.logger.debug(handle)
    #ip = c_char_p("172.16.1.190")
    #port = c_ushort(8000)
    #user_name = c_char_p("admin")
    #user_pwd = c_char_p("12345")
    ##login_ret = handle.NET_DVR_Login_V30(ip, port, user_name, user_pwd, None)
    ##test_logger.logger.debug('loginret:',login_ret)
    ##login = handle.NET_DVR_Login_V40

    ##login.argtypes = [POINTER(login_info), POINTER(deviceinfo_v40)]
    ##login.restype = c_long
    ##login(pointer(hik_login_info), None)
    #login_id = handle.NET_DVR_Login_V40(pointer(hik_login_info), pointer(hik_device_info))
    #ret = c_ulong()
    #channel = c_ulong(0xffffffff)
    #tmp_time = hik_time()
    #tmp_ret = handle.NET_DVR_GetDVRConfig(login_id, 118, channel, pointer(tmp_time), sizeof(hik_time),pointer(ret))
    #test_logger.logger.debug('hik_time:',tmp_ret)
    #test_logger.logger.debug(tmp_time)
    ##login_id = handle.NET_DVR_Login_V40(None, None)
    ##error_code = handle.NET_DVR_GetLastError()
    ##test_logger.logger.debug(error_code)
    #test_logger.logger.debug('longid:', login_id)
    #test_logger.logger.debug(hik_device_info)



