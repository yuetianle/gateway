#!/usr/bin/env python
# -*- coding=utf8 -*-
import requests
import collections
import psia_device_status_client
from multiprocessing import Process, Pipe, RLock
from multiprocessing.managers import BaseManager
from multiprocessing import Lock, freeze_support
from threading import Thread
import Queue, sys, time

try:
    import xml.etree.cElementTree as ET
except:
    import xml.etree.ElementTree as ET

""" use the queue to transport
class QueueManager(BaseManager):
    pass
device_status_queue = Queue.Queue()
def get_device_status_queue():
    global device_status_queue
    return device_status_queue
#freeze_support()
#QueueManager.register('get_device_status_queue', callable=lambda: device_status_queue)
QueueManager.register('get_device_status_queue', callable=get_device_status_queue)
server_manager = QueueManager(address=('127.0.0.1', 60000), authkey='vistek@abc')
connect_server_start = False

def start_connect_server():
    global server_manager
    global connect_server_start
    if not connect_server_start:
        server_manager.start()
        #server = server_manager.get_server()
        #server.serve_forever()
"""
device_info = collections.namedtuple('device_info', 'device_id ip, port, user, pwd')
device_lists = dict()
device_status_lists = dict()
class psia_converter():
    def __init__(self, content, device=None):
        self.content = content
        ET.register_namespace('', 'urn:psialliance-org')
        self.xml_node = ET.fromstring(self.content)
        print(self.xml_node)
        self.psia_ns = {'root_ns':'urn:psialliance-org'}
        if device is not None:
            self.device = device
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

    def to_device_status_xml(self, device):
        #device_info_node = self.xml_node.find('root_ns:DeviceInfo', self.psia_ns)
        #print('device_info:',device_info_node)
        device_status_node = ET.Element('device_status')
        if str(self.xml_node.tag) != '{urn:psialliance-org}DeviceStatus':
            device_status_node.text = 'False'
        else:
            #device_status_node.text = 'True'
            device_status_node.text = str(True)
        device_status_node.set('ip', device.ip)
        device_status_node.set('port', str(device.port))
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
    global device_lists
    global device_status_lists
    print(locals())
    register_node = ET.Element('register')
    ip_node = ET.SubElement(register_node, 'ip')
    ip_node.text = ip
    session_node = ET.SubElement(register_node, 'session')
    if device_id not in device_lists:
        device_lists[device_id] = device_info(device_id=device_id, ip=ip, port=port, user = user_name, pwd = user_pwd)
        device_status_lists[ip] = True
        tmp_lists = psia_device_status_client.get_device_lists()
        tmp_lists.update(device_lists)
        print('items:', tmp_lists.items(), 'keys:', tmp_lists.keys())
        #tmp_lists[device_id] = device_info(device_id=device_id, ip=ip, port=port, user = user_name, pwd = user_pwd)
        tmp_status_lists = psia_device_status_client.get_device_status_lists()
        tmp_status_lists.update(device_status_lists)
        print(tmp_status_lists.items())
        #tmp_status_lists[ip] = True
    register_xml = ET.tostring(register_node, encoding="UTF-8", method="xml")
    print('out:', register_xml, 'len:', len(register_xml))
    return (register_xml, len(register_xml))
def unregister_device(device_id):
    global device_lists
    if device_id in device_lists:
        device_lists.pop(device_id)
def request(device_id, uri, method, timeout=None):
    #global device_lists
    device_lists = psia_device_status_client.get_device_lists()
    print(locals())
    if timeout is None:
        timeout = 5
    if not device_lists.has_key(device_id):
        return None
    else:
        login_info = device_lists.get(device_id)
        print('login_info', login_info)
        request_auth = (device_lists.get(device_id).user, device_lists.get(device_id).pwd)
        response = requests.request(method, uri, auth=request_auth, timeout=timeout)
        if response.status_code == 200:
            return response.text
        else:
            print('error code:', response.status_code, 'text:', response.text)
            return None

def get_stream_url(device_id, channel=None):
    #global device_lists
    #print(locals())
    device_lists = psia_device_status_client.get_device_lists()
    if not device_lists.has_key(device_id):
        return ('', 0)
    tmp_psia_uri = psia_uri_converter('get_stream_url', device_lists.get(device_id))
    tmp_out_data = request(device_id, tmp_psia_uri.psia_uri(), tmp_psia_uri.method())
    if tmp_out_data is None:
        return ('', 0)
    else:
        print(type(tmp_out_data))
        if isinstance(tmp_out_data, unicode):
            xml_converter = psia_converter(tmp_out_data.encode('utf-8'))
        else:
            xml_converter = psia_converter(tmp_out_data)
        out_xml = xml_converter.std_xml('to_stream_url', device_lists.get(device_id))
        return (out_xml, len(out_xml))

def get_device_status(device_id):
    #global device_lists
    #print(locals())
    device_lists = psia_device_status_client.get_device_lists()
    #if device_id not in device_lists:
    if not device_lists.has_key(device_id ):
        print("device not exist, id:", device_id)
        return ("", 0)
    tmp_psia_uri = psia_uri_converter('get_device_status', device_lists.get(device_id))
    tmp_out_data = request(device_id, tmp_psia_uri.psia_uri(), tmp_psia_uri.method())
    if tmp_out_data is None:
        return ("", 0)
    else:
        xml_converter_result = psia_converter(tmp_out_data, device_lists.get(device_id))
        out_xml = xml_converter_result.std_xml('to_device_status_xml', device_lists.get(device_id))
        print('out_xml:', out_xml)
        return (out_xml, len(out_xml))

"""
# working thread   
class Worker(Thread):   
   worker_count = 0   
   def __init__( self, workQueue, resultQueue, timeout = 0, **kwds):   
       Thread.__init__( self, **kwds )   
       self.id = Worker.worker_count   
       Worker.worker_count += 1   
       self.setDaemon( True )   
       self.workQueue = workQueue   
       self.resultQueue = resultQueue   
       self.timeout = timeout   
       self.start( )   
   def run( self ):   
       ''' the get-some-work, do-some-work main loop of worker threads '''   
       while True:   
           try:   
               callable, args, kwds = self.workQueue.get(timeout=self.timeout)   
               res = callable(*args, **kwds)   
               print "worker[%2d]: %s" % (self.id, str(res) )   
               self.resultQueue.put( res )   
           except Queue.Empty:   
               break   
           except :   
               print 'worker[%2d]' % self.id, sys.exc_info()[:2]   
                  
class WorkerManager:   
   def __init__( self, num_of_workers=10, timeout = 1):   
       self.workQueue = Queue.Queue()   
       self.resultQueue = Queue.Queue()   
       self.workers = []   
       self.timeout = timeout   
       self._recruitThreads( num_of_workers )   
   def _recruitThreads( self, num_of_workers ):   
       for i in range( num_of_workers ):   
           worker = Worker( self.workQueue, self.resultQueue, self.timeout )   
           self.workers.append(worker)
   def result_queue_size(self):
       return self.resultQueue.qsize()
   def result_queue_empty(self):
       return self.resultQueue.empty()
   def wait_for_complete( self):   
       # ...then, wait for each of them to terminate:   
       while len(self.workers):   
           worker = self.workers.pop()   
           worker.join( )   
           if worker.isAlive() and not self.workQueue.empty():   
               self.workers.append( worker )   
       print "All jobs are are completed."   
   def add_job( self, callable, *args, **kwds ):   
       self.workQueue.put( (callable, args, kwds) )   
   def get_result( self, *args, **kwds ):   
       return self.resultQueue.get( *args, **kwds )  
def start_device_status_server():
    global device_status_manager
    global device_lists
    global device_status_lists
    global server_manager
    start_connect_server()
    while 1:
        print("come 1")
        device_status_manager = WorkerManager(20, 5)
        for device_id, device_value in device_lists.items():
            device_status_manager.add_job(get_device_status, device_value.device_id)
        device_status_manager.wait_for_complete()

        print(device_status_lists.items())
        out_queue = server_manager.get_device_status_queue();
        while not device_status_manager.result_queue_empty():
            out_str = device_status_manager.get_result()
            device_status_node = ET.fromstring(out_str[0])
            dev_node_ip = device_status_node.get('ip')
            if dev_node_ip in device_status_lists and device_status_node.text != str(device_status_lists.get(dev_node_ip)):
               print(out_str)
               out_queue.put(out_str)

        print('type:', type(out_queue))
        time.sleep(5)
"""
if __name__ == '__main__':
    register_device('111','172.16.1.190',80,'admin','12345')
    register_device('222','172.16.1.191',80,'admin','12345')
    #freeze_support()
    #start_device_status_server()
    print('here')
    #register_device('111','172.16.1.190',80,'admin','12345')
    #out = request('111', 'http://172.16.1.190:80/PSIA/Streaming/channels', 'GET')
    #print('out:', out, 'len:', len(out), 'type:', type(out))
    #out = get_stream_url('111')
    #print('out:', out, 'len:', len(out), 'type:', type(out))
    #out = get_device_status('111')
    #print('out:', out, 'len:', len(out), 'type:', type(out))
    
    #----*  error case *------

    #register_device('111','172.16.1.220',80,'admin','12345')
    #out = request('111', 'http://172.16.1.190:80/PSIA/Streaming/channels', 'GET')
    #print('out:', out, 'len:', len(out), 'type:', type(out))
    #out = get_stream_url('111')
    #print('out:', out, 'len:', len(out), 'type:', type(out))
    #out = get_device_status('111')
    #print('out:', out, 'len:', len(out), 'type:', type(out))
