#!/usr/bin/python

import sys
import io
import os

path = sys.argv[1]

for file in os.listdir(path):
    if file.endswith(".l"):
        if (file[0] != '.'):
            print(os.path.join(path, file))
            print(file)