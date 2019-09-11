python -m timeit -s 'import ctypes; c = ctypes.cdll.LoadLibrary("../gcode/gcparser.so")' 'c.load(b"G0 X10.5 Y10.5 Z10.5")'
