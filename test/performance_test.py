import ctypes
import timeit

ctypes_gcparser = ctypes.cdll.LoadLibrary('../gcode/gcparser.so')

TEST_LINE = b'G0 X10.5 Y10.5 Z10.5'
ctypes_n = 10000

print("Ctypes time",
      timeit.timeit(stmt='ctypes_gcparser.load(TEST_LINE)',
                    setup="from __main__ import ctypes_gcparser, TEST_LINE",
                    number=ctypes_n)/ctypes_n*1000*1000, "µs")
