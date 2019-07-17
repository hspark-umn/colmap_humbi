import os

import cv2
import numpy as np
import math
import time
import random
import matplotlib.pyplot as plt
import glob
from plyfile import PlyData, PlyElement

def SaveStructureData(filename, ply_data):
    print(filename)
    with open(filename, "w") as f:
        data = ply_data.elements[0].data
        f.write("n: %d\n" % len(data))
        for i in range(len(data)):
            f.write("%d %d %d %d %f %f %f\n" % (i, data[i][6], data[i][7], data[i][8], data[i][0], data[i][1], data[i][2]))
        f.close()

def SaveNormalData(filename, ply_data):
    print(filename)
    with open(filename, "w") as f:
        data = ply_data.elements[0].data
        f.write("n: %d\n" % len(data))
        for i in range(len(data)):
            f.write("%d %f %f %f\n" % (i, data[i][3], data[i][4], data[i][5]))
        f.close()



def main():

    for i in range(1, 750, 3):
        file_path = "%08d/mvs/dense/fused.ply" % i
        plydata = PlyData.read(file_path)
        save_path = "fused/structure%08d.txt" % i
        save_path1 = "fused/normal%08d.txt" % i
        SaveStructureData(save_path, plydata)
        SaveNormalData(save_path1, plydata)

if __name__ == '__main__':
    main()