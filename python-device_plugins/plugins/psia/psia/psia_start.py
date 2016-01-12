#!/usr/bin/env python
# -*- coding=utf-8 -*-
import sys
import os
import psia_device_status_server
import subprocess
def start_server():
    script_path = os.path.join(os.path.dirname(psia_device_status_server.__file__), "psia_device_status_server.py")
    print('script_path:', script_path)
    #p = subprocess.Popen(["python","psia_device_status_server.py"])
    p = subprocess.Popen(["python", script_path])
    #p.communicate()
    #p.wait()

if __name__ == '__main__':

    start_server()