#!/usr/bin/env python
# -*- coding: utf-8 -*-
import logging

# 使用一个名字为fib的logger
logger = logging.getLogger('hikvision')
logging.basicConfig(level = logging.DEBUG, format='[%(asctime)s] [%(levelname)s] [%(name)s] [%(filename)s:%(funcName)s:%(lineno)s] : %(message)s', datefmt='%a, %d %b %Y %H:%M:%S', filename='hikvision.log', filemode='w')

# 创建一个输出日志到控制台的StreamHandler
hdr = logging.StreamHandler()
# 给logger添加上handler
logger.addHandler(hdr)
