COLMAP_HUMBI
======

About
-----

This project is designed for dense reconstruction specific to HUMBI dataset. It is mostly identical to COLMAP except for stereo_patch_match where we leverage human segmentation to reduce the computational cost. 

Download
--------

To build COLMAP from source,
please see https://colmap.github.io/install.html.

Place humbi_scripts/* to ~/bin/ and add ~/bin to environment (https://stackoverflow.com/questions/13428910/how-to-set-the-environmental-variable-ld-library-path-in-linux) 

Instruction
--------

Consider a data folder DATA = Humbi/subject_87/.

The script and code run dense stereo matching to produce a 3D model. Given the calibrated cameras in DATA/calibration, you will run the following script (for 00000001 time instant):
cd DATA/body
run_colmap_dense.sh 00000001 

To run a full batch,
run_dense_recon.sh

Full resolution (1920x1080, ~10 min per frame) : run_colmap_dense.sh and run_dense_recon.sh 
Half resolution (960x540, ~3 min per frame) : run_colmap_dense1.sh and run_dense_recon1.sh 

Output:
00000001/mvs/fused.ply
or 00000001/mvs/fused1.ply

License
-------

The COLMAP library is licensed under the new BSD license. Note that this text
refers only to the license for COLMAP itself, independent of its optional
dependencies, which are separately licensed. Building COLMAP with these optional
dependencies may affect the resulting COLMAP license.

    Copyright (c) 2018, ETH Zurich and UNC Chapel Hill.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.

        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.

        * Neither the name of ETH Zurich and UNC Chapel Hill nor the names of
          its contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    Author: Johannes L. Schoenberger (jsch-at-demuc-dot-de)
# colmap_humbi
