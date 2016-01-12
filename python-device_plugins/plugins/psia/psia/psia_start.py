#!/usr/bin/env python
# -*- coding=utf-8 -*-
import sys
import subprocess
def start_server():
    p = subprocess.Popen(["python","psia_device_status_server.py"])
    p.wait()

if __name__ == '__main__':
    start_server()