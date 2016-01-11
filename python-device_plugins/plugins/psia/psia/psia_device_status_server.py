#!/usr/bin/env python
# -*- coding=utf-8 -*-
import psia_wrap
try:
    import xml.etree.cElementTree as ET
except:
    import xml.etree.ElementTree as ET
from multiprocessing.managers import BaseManager
from multiprocessing import Lock, freeze_support
from threading import Thread
import Queue, time
""" use the queue to transport"""
class QueueManager(BaseManager):
    pass
device_status_queue = Queue.Queue()
def get_device_status_queue():
    global device_status_queue
    return device_status_queue
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
    global server_manager
    start_connect_server()
    while 1:
        device_status_manager = WorkerManager(20, 5)
        for device_id, device_value in psia_wrap.device_lists.items():
            device_status_manager.add_job(psia_wrap.get_device_status, device_value.device_id)
        device_status_manager.wait_for_complete()

        print(psia_wrap.device_status_lists.items())
        out_queue = server_manager.get_device_status_queue();
        print('queue:', out_queue)
        while not device_status_manager.result_queue_empty():
            out_str = device_status_manager.get_result()
            device_status_node = ET.fromstring(out_str[0])
            dev_node_ip = device_status_node.get('ip')
            out_queue.put(out_str)
            if dev_node_ip in psia_wrap.device_status_lists and device_status_node.text != str(psia_wrap.device_status_lists.get(dev_node_ip)):
                print(out_str)
                out_queue.put(out_str)

        print('type:', type(out_queue))
        time.sleep(5)
if __name__ == '__main__':
    psia_wrap.register_device('192.168.1.106','192.168.1.106', 80,'admin','12345')
    freeze_support()
    start_device_status_server()
