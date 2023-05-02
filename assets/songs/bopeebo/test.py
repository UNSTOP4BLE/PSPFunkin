#!/usr/bin/env python3

import sys
from struct import Struct

HEADER = Struct("< 16s 2d 2i")
SECTION = Struct("< i")
NOTE = Struct("< f i f i")

with open(sys.argv[1], "rb") as f:
    magic, speed, bpm, sectioncount, notecount = HEADER.unpack(f.read(HEADER.size))
    print("header", magic, speed, bpm, sectioncount, notecount)

    for i in range(sectioncount):
        flag, = SECTION.unpack(f.read(SECTION.size))
        print("section", i, hex(flag))

    for i in range(notecount):
        pos, _type, sus, flag, = NOTE.unpack(f.read(NOTE.size))
        print("note", i, pos, _type, sus, hex(flag))