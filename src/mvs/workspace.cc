// Copyright (c) 2018, ETH Zurich and UNC Chapel Hill.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of ETH Zurich and UNC Chapel Hill nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: Johannes L. Schoenberger (jsch-at-demuc-dot-de)

#include "mvs/workspace.h"

#include <numeric>

#include "util/misc.h"

namespace colmap {
namespace mvs {

Workspace::CachedImage::CachedImage() {}

Workspace::CachedImage::CachedImage(CachedImage&& other) {
  num_bytes = other.num_bytes;
  bitmap = std::move(other.bitmap);
  depth_map = std::move(other.depth_map);
  normal_map = std::move(other.normal_map);
}

Workspace::CachedImage& Workspace::CachedImage::operator=(CachedImage&& other) {
  if (this != &other) {
    num_bytes = other.num_bytes;
    bitmap = std::move(other.bitmap);
    depth_map = std::move(other.depth_map);
    normal_map = std::move(other.normal_map);
  }
  return *this;
}

size_t Workspace::CachedImage::NumBytes() const { return num_bytes; }

Workspace::Workspace(const Options& options)
    : options_(options),
      cache_(1024 * 1024 * 1024 * options_.cache_size,
             [](const int) { return CachedImage(); }) {
  StringToLower(&options_.input_type);
  model_.Read(options_.workspace_path, options_.workspace_format);
  if (options_.max_image_size > 0) {
    for (auto& image : model_.images) {
      image.Downsize(options_.max_image_size, options_.max_image_size);
    }
  }

  maskmap_path_ = EnsureTrailingSlash(
      JoinPaths(options_.workspace_path, "../masks"));

  depth_map_path_ = EnsureTrailingSlash(
      JoinPaths(options_.workspace_path, options_.stereo_folder, "depth_maps"));
  normal_map_path_ = EnsureTrailingSlash(JoinPaths(
      options_.workspace_path, options_.stereo_folder, "normal_maps"));
}

void Workspace::ClearCache() { cache_.Clear(); }

const Workspace::Options& Workspace::GetOptions() const { return options_; }

const Model& Workspace::GetModel() const { return model_; }

const Bitmap& Workspace::GetBitmap(const int image_idx) {
  auto& cached_image = cache_.GetMutable(image_idx);
  if (!cached_image.bitmap) {
    cached_image.bitmap.reset(new Bitmap());
    cached_image.bitmap->Read(GetBitmapPath(image_idx), options_.image_as_rgb);
    //std::cout << GetBitmapPath(image_idx).c_str() << " " << cached_image.bitmap->Width() << std::endl;
    if (options_.max_image_size > 0) {
      cached_image.bitmap->Rescale(model_.images.at(image_idx).GetWidth(),
                                   model_.images.at(image_idx).GetHeight());
    }
    cached_image.num_bytes += cached_image.bitmap->NumBytes();
    cache_.UpdateNumBytes(image_idx);
  }
  return *cached_image.bitmap;
}
/*
const Bitmap& Workspace::GetMaskMap(const int image_idx) {
  auto& cached_image = cache_.GetMutable(image_idx);
  if (!cached_image.maskmap) {
    cached_image.bitmap.reset(new Bitmap());
    string mask_path = JoinPaths(options_.workspace_path, "../../masks/", GetBitmapPath(image_idx));
	mask_path += ".png";
    printf("%s\n", mask_path);
    cached_image.maskmap->Read(mask_path.c_str());
    if (options_.max_image_size > 0) {
      cached_image.maskmap->Rescale(model_.images.at(image_idx).GetWidth(),
                                   model_.images.at(image_idx).GetHeight());
    }
    cached_image.num_bytes += cached_image.maskmap->NumBytes();
    cache_.UpdateNumBytes(image_idx);
  }
  return *cached_image.maskmap;
}*/

const DepthMap Workspace::GetIntegralImage(Bitmap bitmap)
{
  DepthMap integral(bitmap.Width(), bitmap.Height(), 0, 1e+8);
  integral.Fill(0.0f);
  BitmapColor<uint8_t> color;
  for (size_t i1 = 0; i1 < bitmap.Height(); i1++)
  {
	for (size_t i2 = 0; i2 < bitmap.Width(); i2++)
	{
		bitmap.GetPixel(i2, i1, &color);
		int value = 0;
		if (color.r != 0)
			value = 1;
		if (i1 > 0)
		{
			float v = integral.Get(i1-1, i2);
			value += (int)v;
		}
		if (i2 > 0)
		{
			float v = integral.Get(i1, i2-1);
			value += (int)v;
		}
		if ((i1 > 0) && (i2 > 0))
		{
			float v = integral.Get(i1-1, i2-1);
			value -= (int)v;
		}
		integral.Set(i1, i2, value);
	}
  }
  return integral;
}

void Workspace::RotateBitmap(Bitmap bitmap, Bitmap& bitmap1)
{
  bitmap1.Allocate(bitmap.Height(), bitmap.Width(), true);
  BitmapColor<uint8_t> color;
  for (size_t i1 = 0; i1 < bitmap.Height(); i1++)
  {
	for (size_t i2 = 0; i2 < bitmap.Width(); i2++)
	{
		bitmap.GetPixel(i2, i1, &color);
		bitmap1.SetPixel(i1, bitmap.Width()-i2-1, color);
	}
  }
}

void Workspace::GetMaskMap(const int image_idx, std::vector<DepthMap>& v_maskmap) {
  auto& cached_image = cache_.GetMutable(image_idx);
  Bitmap bitmap1;
std::cout << GetMaskMapPath(image_idx).c_str() << std::endl;
  CHECK(bitmap1.Read(GetMaskMapPath(image_idx).c_str()));
  bitmap1.Rescale(cached_image.bitmap->Width(), cached_image.bitmap->Height());

  //Bitmap bitmap1(bitmap);
  //bitmap1.Smooth(10, 10);
/*  bitmap1.Allocate(bitmap.Height(), bitmap.Width(), true);
  BitmapColor<uint8_t> color;
  for (int i1 = 0; i1 < bitmap.Height(); i1++)
  {
	for (int i2 = 0; i2 < bitmap.Width(); i2++)
	{
	    bool isGood = false;
	    for (int i11 = -5; i11 <= 5; i11++)
	    {
	        if ((i1+i11 < 0) || (i1+i11 >= bitmap.Height()))
	            continue;
	        for (int i22 = -5; i22 <= 5; i22++)
	        {
	            if ((i2+i22 < 0) || (i2+i22 >= bitmap.Width()))
	                continue;
	            bitmap.GetPixel(i2+i22, i1+i11, &color);
	            if (color.r != 0)
	            {
	                isGood = true;
	                break;
	            }
	        }
	        if (isGood)
	            break;
	    }
        bitmap1.SetPixel(i2, i1, color);
        if (isGood)
        {
            color.r = 1;
            bitmap1.SetPixel(i2, i1, color);
        }
        else
        {
            color.r = 0;
            bitmap1.SetPixel(i2, i1, color);
        }

	}
  }*/

  //std::vector<DepthMap> v_maskmap;
  DepthMap integral = GetIntegralImage(bitmap1);
  //bitmap.Write("/home/hspark/bit1.png");
    if (options_.max_image_size > 0) {
      integral.Downsize(model_.images.at(image_idx).GetWidth(),
                                     model_.images.at(image_idx).GetHeight());
    }
//std::cout << "pushing" << std::endl;
    v_maskmap.push_back(integral);
    //std::cout << "pushing" << std::endl;

    Bitmap bitmap2;
    RotateBitmap(bitmap1, bitmap2);
    //std::cout << "a" << std::endl;
    //std::cout << bitmap1.Width() << std::endl;
    integral = GetIntegralImage(bitmap2);
    if (options_.max_image_size > 0) {
      integral.Downsize(model_.images.at(image_idx).GetWidth(),
                                     model_.images.at(image_idx).GetHeight());
    }
    v_maskmap.push_back(integral);

    Bitmap bitmap3;
    RotateBitmap(bitmap2, bitmap3);
    integral = GetIntegralImage(bitmap3);
    if (options_.max_image_size > 0) {
      integral.Downsize(model_.images.at(image_idx).GetWidth(),
                                     model_.images.at(image_idx).GetHeight());
    }
    v_maskmap.push_back(integral);

    Bitmap bitmap4;
    RotateBitmap(bitmap3, bitmap4);
    integral = GetIntegralImage(bitmap4);
    if (options_.max_image_size > 0) {
      integral.Downsize(model_.images.at(image_idx).GetWidth(),
                                     model_.images.at(image_idx).GetHeight());
    }
    v_maskmap.push_back(integral);

//std::cout << "deac" << std::endl;
    bitmap1.Deallocate();
    bitmap2.Deallocate();
    bitmap3.Deallocate();
    bitmap4.Deallocate();
    std::cout << GetMaskMapPath(image_idx).c_str() << std::endl;
//std::cout << "mask loaded" << std::endl;
/*
    bitmap1.Write("/home/hspark/bit1.png");
    bitmap2.Write("/home/hspark/bit2.png");
    bitmap3.Write("/home/hspark/bit3.png");
    bitmap4.Write("/home/hspark/bit4.png");
*/
    //bitmap1.Deallocate();


 /*
    cached_image.maskmap.reset(new DepthMap(bitmap.Width(), bitmap.Height(), 0, 1e+8));
    //std::cout << GetMaskMapPath(image_idx).c_str() << " " << cached_image.maskmap->GetWidth() << std::endl;
    
    //cached_image.maskmap->Read(GetMaskMapPath(image_idx));
 
    

    cached_image.maskmap->Fill(0.0f);
  //cached_image.maskmap = new(bitmap.Width(), bitmap.Height(), 0, 1e+8);
  //integral_image.Fill(0.0f);







  Bitmap bitmap1;
  bitmap1.Rescale(cached_image.bitmap->Height(), cached_image.bitmap->Width());


  //Bitmap integral = cached_image.maskmap->ToBitmap(0, 100);
  //integral.Write("/home/hspark/mask.png");

    */


	//const float factor_x = static_cast<float>(cached_image.bitmap->Width()) / cached_image.maskmap->GetWidth();
  //const float factor_y = static_cast<float>(cached_image.bitmap->Height()) / cached_image.maskmap->GetHeight();
    //cached_image.maskmap->Rescale(std::min(factor_x, factor_y));

    //cached_image.maskmap->Downsize(cached_image.bitmap->Width(),
    //                               cached_image.bitmap->Height());

    //std::cout << GetMaskMapPath(image_idx).c_str() << " " << cached_image.maskmap->GetWidth() << std::endl;

    //cached_image.num_bytes += cached_image.maskmap->GetNumBytes();
    //cache_.UpdateNumBytes(image_idx);
  //}
  //return v_maskmap;
}


void Workspace::GetMaskMap1(const int image_idx, int &block_size_y, std::vector<int> &v_width0, std::vector<int> &v_height0,
                            std::vector<std::vector<int> >& v_coord_y, std::vector<std::vector<std::pair<int, int> > >& v_coord_x) {
  auto& cached_image = cache_.GetMutable(image_idx);
  std::string str = GetMaskMapPath(image_idx).c_str();
  str += ".txt";
  std::cout << str << std::endl;

  //std::ifstream file(str);
  std::ifstream fin;
  fin.open(str.c_str(), std::ifstream::in);
  std::string dummy;
  fin >> dummy >> block_size_y;
  int n, w, h;
  fin >> n >> w >> h;
  v_width0.push_back(w);
  v_height0.push_back(h);
  std::vector<int> coord_y1, coord_y2, coord_y3, coord_y4;
  std::vector<std::pair<int, int> > coord_x1, coord_x2, coord_x3, coord_x4;
  for (int i = 0; i < n; i++)
  {
    int m;
    int coord_y;
    fin >> m >> coord_y;

    for (int j = 0; j < m; j++)
    {
        std::pair<int, int> interval;
        int starting, ending;
        fin >> starting >> ending;
        interval.first = starting;
        interval.second = ending;
        coord_y1.push_back(coord_y);
        coord_x1.push_back(interval);
    }
  }

  fin >> n >> w >> h;
  v_width0.push_back(w);
  v_height0.push_back(h);
  for (int i = 0; i < n; i++)
  {
    int m;
    int coord_y;
    fin >> m >> coord_y;

    for (int j = 0; j < m; j++)
    {
        std::pair<int, int> interval;
        int starting, ending;
        fin >> starting >> ending;
        interval.first = starting;
        interval.second = ending;
        coord_y2.push_back(coord_y);
        coord_x2.push_back(interval);
    }
  }

  fin >> n >> w >> h;
  v_width0.push_back(w);
  v_height0.push_back(h);
  for (int i = 0; i < n; i++)
  {
    int m;
    int coord_y;
    fin >> m >> coord_y;

    for (int j = 0; j < m; j++)
    {
        std::pair<int, int> interval;
        int starting, ending;
        fin >> starting >> ending;
        interval.first = starting;
        interval.second = ending;
        coord_y3.push_back(coord_y);
        coord_x3.push_back(interval);
    }
  }

  fin >> n >> w >> h;
  v_width0.push_back(w);
  v_height0.push_back(h);
  for (int i = 0; i < n; i++)
  {
    int m;
    int coord_y;
    fin >> m >> coord_y;

    for (int j = 0; j < m; j++)
    {
        std::pair<int, int> interval;
        int starting, ending;
        fin >> starting >> ending;
        interval.first = starting;
        interval.second = ending;
        coord_y4.push_back(coord_y);
        coord_x4.push_back(interval);
    }
  }

  v_coord_y.push_back(coord_y1);
  v_coord_y.push_back(coord_y2);
  v_coord_y.push_back(coord_y3);
  v_coord_y.push_back(coord_y4);

  v_coord_x.push_back(coord_x1);
  v_coord_x.push_back(coord_x2);
  v_coord_x.push_back(coord_x3);
  v_coord_x.push_back(coord_x4);

  fin.close();
}

const DepthMap& Workspace::GetDepthMap(const int image_idx) {
  auto& cached_image = cache_.GetMutable(image_idx);
  if (!cached_image.depth_map) {
    cached_image.depth_map.reset(new DepthMap());
    cached_image.depth_map->Read(GetDepthMapPath(image_idx));
    if (options_.max_image_size > 0) {
      cached_image.depth_map->Downsize(model_.images.at(image_idx).GetWidth(),
                                       model_.images.at(image_idx).GetHeight());
    }
    cached_image.num_bytes += cached_image.depth_map->GetNumBytes();
    cache_.UpdateNumBytes(image_idx);
  }
  return *cached_image.depth_map;
}

const NormalMap& Workspace::GetNormalMap(const int image_idx) {
  auto& cached_image = cache_.GetMutable(image_idx);
  if (!cached_image.normal_map) {
    cached_image.normal_map.reset(new NormalMap());
    cached_image.normal_map->Read(GetNormalMapPath(image_idx));
    if (options_.max_image_size > 0) {
      cached_image.normal_map->Downsize(
          model_.images.at(image_idx).GetWidth(),
          model_.images.at(image_idx).GetHeight());
    }
    cached_image.num_bytes += cached_image.normal_map->GetNumBytes();
    cache_.UpdateNumBytes(image_idx);
  }
  return *cached_image.normal_map;
}

std::string Workspace::GetBitmapPath(const int image_idx) const {
  return model_.images.at(image_idx).GetPath();
}

std::string Workspace::GetDepthMapPath(const int image_idx) const {
  return depth_map_path_ + GetFileName(image_idx);
}

std::string Workspace::GetMaskMapPath(const int image_idx) const {
  return maskmap_path_ + model_.GetImageName(image_idx) + ".png";
}

std::string Workspace::GetNormalMapPath(const int image_idx) const {
  return normal_map_path_ + GetFileName(image_idx);
}

bool Workspace::HasBitmap(const int image_idx) const {
  return ExistsFile(GetBitmapPath(image_idx));
}

bool Workspace::HasDepthMap(const int image_idx) const {
  return ExistsFile(GetDepthMapPath(image_idx));
}

bool Workspace::HasNormalMap(const int image_idx) const {
  return ExistsFile(GetNormalMapPath(image_idx));
}

std::string Workspace::GetFileName(const int image_idx) const {
  const auto& image_name = model_.GetImageName(image_idx);
  return StringPrintf("%s.%s.bin", image_name.c_str(),
                      options_.input_type.c_str());
}

void ImportPMVSWorkspace(const Workspace& workspace,
                         const std::string& option_name) {
  const std::string& workspace_path = workspace.GetOptions().workspace_path;
  const std::string& stereo_folder = workspace.GetOptions().stereo_folder;

  CreateDirIfNotExists(JoinPaths(workspace_path, stereo_folder));
  CreateDirIfNotExists(JoinPaths(workspace_path, stereo_folder, "depth_maps"));
  CreateDirIfNotExists(JoinPaths(workspace_path, stereo_folder, "normal_maps"));
  CreateDirIfNotExists(
      JoinPaths(workspace_path, stereo_folder, "consistency_graphs"));

  const auto option_lines =
      ReadTextFileLines(JoinPaths(workspace_path, option_name));
  for (const auto& line : option_lines) {
    if (!StringStartsWith(line, "timages")) {
      continue;
    }

    const auto elems = StringSplit(line, " ");
    int num_images = std::stoull(elems[1]);

    std::vector<int> image_idxs;
    if (num_images == -1) {
      CHECK_EQ(elems.size(), 4);
      const int range_lower = std::stoull(elems[2]);
      const int range_upper = std::stoull(elems[3]);
      CHECK_LT(range_lower, range_upper);
      num_images = range_upper - range_lower;
      image_idxs.resize(num_images);
      std::iota(image_idxs.begin(), image_idxs.end(), range_lower);
    } else {
      CHECK_EQ(num_images + 2, elems.size());
      image_idxs.reserve(num_images);
      for (size_t i = 2; i < elems.size(); ++i) {
        const int image_idx = std::stoull(elems[i]);
        image_idxs.push_back(image_idx);
      }
    }

    std::vector<std::string> image_names;
    image_names.reserve(num_images);
    for (const auto image_idx : image_idxs) {
      const std::string image_name =
          workspace.GetModel().GetImageName(image_idx);
      image_names.push_back(image_name);
    }

    const auto& overlapping_images =
        workspace.GetModel().GetMaxOverlappingImagesFromPMVS();

    const auto patch_match_path =
        JoinPaths(workspace_path, stereo_folder, "patch-match.cfg");
    const auto fusion_path =
        JoinPaths(workspace_path, stereo_folder, "fusion.cfg");
    std::ofstream patch_match_file(patch_match_path, std::ios::trunc);
    std::ofstream fusion_file(fusion_path, std::ios::trunc);
    CHECK(patch_match_file.is_open()) << patch_match_path;
    CHECK(fusion_file.is_open()) << fusion_path;
    for (size_t i = 0; i < image_names.size(); ++i) {
      const auto& ref_image_name = image_names[i];
      patch_match_file << ref_image_name << std::endl;
      if (overlapping_images.empty()) {
        patch_match_file << "__auto__, 20" << std::endl;
      } else {
        for (const int image_idx : overlapping_images[i]) {
          patch_match_file << workspace.GetModel().GetImageName(image_idx)
                           << ", ";
        }
        patch_match_file << std::endl;
      }
      fusion_file << ref_image_name << std::endl;
    }
  }
}

}  // namespace mvs
}  // namespace colmap
