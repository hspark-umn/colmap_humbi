#!/bin/bash

FRAME_PATH=$1
IMAGE_PATH=../image/
PROJECT_PATH=../../../calibration/sparse/
#PROJECT_PATH=~/ColmapTest1/sparse/0/
cd $FRAME_PATH
echo $FRAME_PATH
mkdir mvs
cd mvs
rm -r dense1
mkdir masks
cp ../segmentation/appearance_result_mask/* masks/
cp ~/bin/mask_for_dense1.py .

colmap image_undistorter --image_path $IMAGE_PATH --input_path $PROJECT_PATH --output_path dense1 --output_type COLMAP --max_image_size 1920

#cp $IMAGE_PATH/* dense/images/

python mask_for_dense1.py

colmap patch_match_stereo \
    --workspace_path dense1 \
    --workspace_format COLMAP \
    --PatchMatchStereo.max_image_size 960 \
    --PatchMatchStereo.window_radius 10 \
    --PatchMatchStereo.window_step 2 \
    --PatchMatchStereo.geom_consistency true

#    --PatchMatchStereo.num_iterations 5 \
#    --PatchMatchStereo.filter_min_ncc 0.1 \
#    --PatchMatchStereo.filter_min_num_consistent 2 \
#    --PatchMatchStereo.filter_geom_consistency_max_cost 2 \
#    --PatchMatchStereo.num_samples 8 \
#    --PatchMatchStereo.geom_consistency true


colmap stereo_fusion \
    --workspace_path dense1 \
    --workspace_format COLMAP \
    --input_type geometric \
    --StereoFusion.max_num_pixels 100000 \
    --output_path fused1.ply

 #   --StereoFusion.max_traversal_depth 500 \
 #   --StereoFusion.max_reproj_error 10 \
 #   --StereoFusion.max_depth_error 0.01 \
 #   --StereoFusion.max_normal_error 10 \
 #   --StereoFusion.check_num_images 20 \
 #   --output_path dense/fused.ply

rm -r dense1
