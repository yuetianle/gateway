#!/usr/bin/env python
# -*- coding: utf-8 -*-
import logging

# 使用一个名字为fib的logger
logger = logging.getLogger('onvif_host')

# 设置logger的level为DEBUG
logger.setLevel(logging.DEBUG)
#logger.setLevel(logging.WARN)

# 创建一个输出日志到控制台的StreamHandler
hdr = logging.StreamHandler()
formatter = logging.Formatter('[%(asctime)s] [%(levelname)s] [%(name)s] [%(filename)s:%(funcName)s:%(lineno)s] : %(message)s')
hdr.setFormatter(formatter)
# 给logger添加上handler
logger.addHandler(hdr)
