"""
Gcode needs to be parsed into logical groups

line-number <-> gcode

block
- code_lines
- opengl geometry

"""
from enum import Enum


class CoordinateMode(Enum):
    UNKNOWN = 0
    ABSOLUTE = 1 # G90
    RELATIVE = 2 # G91

class CoordinateUnit(Enum):
    UNKNOWN = 0
    INCHES = 1      # G20
    MILLIMETERS = 2 # G21

class FeedMode(Enum):
    UNKNOWN = 0
    MOVE = 1
    RAPID = 2
    CLOCKWISE_ARC = 3
    COUNTERCLOCKWISE_ARC = 5

class ProgramState(object):
    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0
        self.coordinate_mode = CoordinateMode.UNKNOWN
        self.coordinate_unit = CoordinateUnit.UNKNOWN
        self.feed_mode = FeedMode.UNKNOWN


class Program(object):
    def __init__(self):
        self.reset()

    def reset(self):
        self.blocks = []

    def load(self, lines):
        self.reset()
        for line in lines:


class Block(object):
    def __init__(self):
        self.lines = []

class Line(object):
    def __init__(self):
        self.valid


