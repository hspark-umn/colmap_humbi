import os
import sys
import collections
import numpy as np
import struct

CameraModel = collections.namedtuple(
    "CameraModel", ["model_id", "model_name", "num_params"])
Camera = collections.namedtuple(
    "Camera", ["id", "model", "width", "height", "params"])
BaseImage = collections.namedtuple(
    "Image", ["id", "qvec", "tvec", "camera_id", "name", "xys", "point3D_ids"])
Point3D = collections.namedtuple(
    "Point3D", ["id", "xyz", "rgb", "error", "image_ids", "point2D_idxs"])

CAMERA_MODELS = {
    CameraModel(model_id=0, model_name="SIMPLE_PINHOLE", num_params=3),
    CameraModel(model_id=1, model_name="PINHOLE", num_params=4),
    CameraModel(model_id=2, model_name="SIMPLE_RADIAL", num_params=4),
    CameraModel(model_id=3, model_name="RADIAL", num_params=5),
    CameraModel(model_id=4, model_name="OPENCV", num_params=8),
    CameraModel(model_id=5, model_name="OPENCV_FISHEYE", num_params=8),
    CameraModel(model_id=6, model_name="FULL_OPENCV", num_params=12),
    CameraModel(model_id=7, model_name="FOV", num_params=5),
    CameraModel(model_id=8, model_name="SIMPLE_RADIAL_FISHEYE", num_params=4),
    CameraModel(model_id=9, model_name="RADIAL_FISHEYE", num_params=5),
    CameraModel(model_id=10, model_name="THIN_PRISM_FISHEYE", num_params=12)
}
CAMERA_MODEL_IDS = dict([(camera_model.model_id, camera_model) \
                         for camera_model in CAMERA_MODELS])

def read_cameras_text(path):
    """
    see: src/base/reconstruction.cc
        void Reconstruction::WriteCamerasText(const std::string& path)
        void Reconstruction::ReadCamerasText(const std::string& path)
    """
    cameras = []
    with open(path, "r") as fid:
        while True:
            line = fid.readline()
            if not line:
                break
            line = line.strip()
            cam = []
            if len(line) > 0 and line[0] != "#":
                elems = line.split()
                camera_id = int(elems[0])
                width = int(elems[2])
                height = int(elems[3])
                params = np.array(tuple(map(float, elems[4:])))
                cam = [camera_id, width, height, params[0], params[1], params[2]]
                # cam.camera = Camera(id=camera_id, model=model,
                #                             width=width, height=height,
                #                             params=params)
                cameras.append(cam)
    return cameras

def write_cameras_text(path, cameras):
    """
    see: src/base/reconstruction.cc
        void Reconstruction::WriteCamerasText(const std::string& path)
        void Reconstruction::ReadCamerasText(const std::string& path)
    """
    with open(path, "w") as fid:
        for i in range(len(cameras)):
            fid.write("%d SIMPLE_PINHOLE %d %d %f %d %d\n" % (cameras[i][0], cameras[i][1], cameras[i][2], cameras[i][3], cameras[i][4], cameras[i][5]))
        fid.close()



def SaveIntrinsicData(filename, cameras):
    print(filename)
    with open(filename, "w") as f:
        f.write("nCameras: %d\n" % 1)
        f.write("nFrames: %d\n" % len(cameras))
        f.write("nPs: %d\n" % len(cameras))
        for i in range(len(cameras)):
            f.write("0 %d\n" % cameras[i].id)
            f.write("%f 0 %f\n" % (cameras[i].params[0], cameras[i].params[1]))
            f.write("0 %f %f\n" % (cameras[i].params[0], cameras[i].params[2]))
            f.write("0 0 1\n")
        f.close()


def main():
    # if len(sys.argv) != 3:
    #     print("Usage: python read_model.py path/to/model/folder [.txt,.bin]")
    #     return
    cameras = read_cameras_text("cameras.txt")

    write_cameras_text("pinhole/cameras.txt", cameras)



if __name__ == "__main__":
    main()