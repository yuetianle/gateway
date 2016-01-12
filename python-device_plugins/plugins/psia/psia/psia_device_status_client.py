#!/usr/bin/env python
#-*- coding=utf-8 -*-
__author__ = '95'
import time, sys, Queue
from multiprocessing.managers import BaseManager
class QueueManager(BaseManager):
    pass
def start_device_status_client():
    QueueManager.register('get_device_status_queue')
    server_addr = '127.0.0.1'
    client = QueueManager(address=(server_addr, 60000), authkey='vistek@abc')

    ret = client.connect()
    print('ret:', ret)
    result_queue = client.get_device_status_queue()
    print('result_queue:', result_queue, 'size:', result_queue.qsize())
    while 1:
        if not result_queue.empty():
            print("outqueue:", result_queue.get_nowait())
        time.sleep(0.01)

if __name__ == '__main__':
    start_device_status_client()