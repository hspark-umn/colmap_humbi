#!/bin/bash

cd calibration
cp ~/bin/radial2pinhole.py .
mkdir pinhole
cp points3D.txt images.txt pinhole
python radial2pinhole.py


