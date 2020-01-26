#!/usr/bin/env python3

# Copyright (C) 2019 Marco Lochen
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from PIL import Image
import sys

def save_img(data):
    if (len(data) != 18 * 18 * 2):
        return False

    try:
        pixels = [ int(data[i:i+2], 16) for i in range(0, len(data), 2) ]
    except ValueError:
        return False

    img = Image.new("L", (18, 18))
    for x in range(18):
        for y in range(18):
            img.putpixel((x, 17 - y), pixels[x * 18 + y] % 64 * 4)
    img.save("scan.png")

    return True

filename = sys.argv[1]

with open(filename) as f:
    while True:
        line = f.readline()
        if (save_img(line.strip())):
            break

