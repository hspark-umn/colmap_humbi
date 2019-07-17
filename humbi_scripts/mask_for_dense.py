import os

import cv2
import numpy as np
import math
import time
import random
import matplotlib.pyplot as plt
import glob

def SaveCoordData(filename, block_size_y, cols, rows, coord_x1, coord_y1, coord_x2, coord_y2, coord_x3, coord_y3, coord_x4, coord_y4):
    print(filename)
    with open(filename, "w") as f:
        f.write("Block_size: %d\n" % block_size_y)
        # f.write("Image_size: %d %d\n" % (cols, rows))
        f.write("%d %d %d\n" % (len(coord_y1), cols[0], rows[0]))

        for i in range(len(coord_y1)):
            f.write("%d %d " % (len(coord_x1[i]), coord_y1[i]))
            for j in range(len(coord_x1[i])):
                f.write("%d %d " % (coord_x1[i][j][0], coord_x1[i][j][1]))
            f.write("\n")

        f.write("%d %d %d\n" % (len(coord_y2), cols[1], rows[1]))

        for i in range(len(coord_y2)):
            f.write("%d %d " % (len(coord_x2[i]), coord_y2[i]))
            for j in range(len(coord_x2[i])):
                f.write("%d %d " % (coord_x2[i][j][0], coord_x2[i][j][1]))
            f.write("\n")

        f.write("%d %d %d\n" % (len(coord_y3), cols[2], rows[2]))
        for i in range(len(coord_y3)):
            f.write("%d %d " % (len(coord_x3[i]), coord_y3[i]))
            for j in range(len(coord_x3[i])):
                f.write("%d %d " % (coord_x3[i][j][0], coord_x3[i][j][1]))
            f.write("\n")

        f.write("%d %d %d\n" % (len(coord_y4), cols[3], rows[3]))
        for i in range(len(coord_y4)):
            f.write("%d %d " % (len(coord_x4[i]), coord_y4[i]))
            for j in range(len(coord_x4[i])):
                f.write("%d %d " % (coord_x4[i][j][0], coord_x4[i][j][1]))
            f.write("\n")
        f.close()

def GetBoundingBox(gray, block_size_y):

    # gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (11, 11), 0)
    # binary = gray
    binary = cv2.threshold(gray, 0.1, 1, cv2.THRESH_BINARY)
    integral = cv2.integral(binary[1])

    [h, w] = np.shape(integral)

    nGrid_y = int(h / block_size_y) + 1

    coord_y1 = []
    coord_x1 = []
    for i1 in range(nGrid_y):
        y = i1 * block_size_y
        y1 = y - 1
        y2 = (i1 + 1) * block_size_y
        y2 = min(y2, h - 1)

        coord_y = y
        coord_x = []
        for i2 in range(w):
            x = i2
            x1 = x - 1
            x2 = x
            if (x1 < 0) and (y1 < 0):
                v11 = 0
                v12 = 0
                v21 = 0
            elif (x1 >= 0) and (y1 < 0):
                v11 = 0
                v12 = 0
                v21 = integral[y2][x1]
            elif (x1 < 0) and (y1 >= 0):
                v11 = 0
                v12 = integral[y1][x2]
                v21 = 0
            else:
                v11 = integral[y1][x1]
                v12 = integral[y1][x2]
                v21 = integral[y2][x1]
            v22 = integral[y2][x2]
            value = v22 - v12 - v21 + v11

            if (value < 1.0):
                continue
            coord_x.append(x)

        if len(coord_x) == 0:
            continue
        ss = []
        starting = coord_x[0]
        for i2 in range(1, len(coord_x)):
            if coord_x[i2 - 1] + 1 != coord_x[i2]:
                ending = coord_x[i2 - 1]
                ss.append([starting, ending])
                starting = coord_x[i2]
            elif i2 == len(coord_x) - 1:
                ending = coord_x[i2]
                ss.append([starting, ending])
        if len(ss) != 0:
            coord_y1.append(y)
            coord_x1.append(ss)

    return coord_x1, coord_y1

def Rotate(im):
    rows, cols = im.shape
    pts1 = np.float32([[0, 0], [0, rows], [cols, rows]])
    pts2 = np.float32([[0, cols], [rows, cols], [rows, 0]])

    M = cv2.getAffineTransform(pts1, pts2)
    im = cv2.warpAffine(im, M, (rows, cols))
    return im


def adjust_gamma(image, gamma=1.0):
    # build a lookup table mapping the pixel values [0, 255] to
    # their adjusted gamma values
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")

    # apply gamma correction using the lookup table
    return cv2.LUT(image, table)


def main():
    file_list = sorted(glob.glob("dense/images/*.jpg"))

    for i in range(len(file_list)):
        im = cv2.imread(file_list[i])
        im_new = adjust_gamma(im, gamma=2.5)
        cv2.imwrite(file_list[i], im_new)


        #
        # plt.figure(1)
        # plt.imshow(im),plt.title('Original')
        # plt.figure(2)
        # plt.imshow(im_new)
        # # # plt.figure(2)
        # # # plt.imshow(integral),plt.title('Blurred')
        # # for i1 in range(len(coord_y)):
        # #     for i2 in range(len(coord_x[i1])):
        # #         ss = coord_x[i1][i2]
        # #         y1 = coord_y[i1]
        # #         y2 = y1 + block_size_y
        # #         plt.plot([ss[0], ss[0], ss[1], ss[1], ss[0]], [y1, y2, y2, y1, y1])
        # #
        # plt.show()
        # cv2.waitKey()


    file_list = sorted(glob.glob("masks/*.png"))

    block_size_y = 100
    for i in range(len(file_list)):
        im = cv2.imread(file_list[i], 0)
        w = []
        h = []
        rows, cols = im.shape
        w.append(cols)
        h.append(rows)

        coord_x1, coord_y1 = GetBoundingBox(im, block_size_y)


        coord_x = coord_x1
        coord_y = coord_y1





        im = Rotate(im)
        rows, cols = im.shape
        w.append(cols)
        h.append(rows)
        coord_x2, coord_y2 = GetBoundingBox(im, block_size_y)
        im = Rotate(im)
        rows, cols = im.shape
        w.append(cols)
        h.append(rows)
        coord_x3, coord_y3 = GetBoundingBox(im, block_size_y)
        im = Rotate(im)
        rows, cols = im.shape
        w.append(cols)
        h.append(rows)
        coord_x4, coord_y4 = GetBoundingBox(im, block_size_y)

        coord_x = coord_x4
        coord_y = coord_y4

        SaveCoordData(file_list[i][:-3]+"jpg.png.txt", block_size_y, w, h, coord_x1, coord_y1, coord_x2, coord_y2, coord_x3, coord_y3, coord_x4, coord_y4)
        #
        # # cv2.imwrite(file_list[i][:-3]+"jpg.png", binary[1])
        #


if __name__ == '__main__':
    main()
