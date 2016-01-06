#!/usr/bin/env python
# -*- coding:utf-8 -*-
import os
from setuptools import setup, find_packages


"""
pisa package Wrap package
"""

here = os.path.abspath(os.path.dirname(__file__))
version_path = os.path.join(here, 'psia/version.txt')
version = open(version_path).read().strip()
requires = ['requests', 'urlobject']
setup(
    name = 'psia',
    version = version,
    packages = find_packages(),
    author = 'lishida',
    author_email = 'shida23577@hotmail.com',
    url = '',
    install_requires=requires,
    license = 'http://www.apache.org/licenses/LICENSE-2.0.html',
    description = 'psia SDK wrap'
        )
