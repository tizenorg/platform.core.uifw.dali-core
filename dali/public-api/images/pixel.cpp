/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali/public-api/images/pixel.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

bool Pixel::HasAlpha(Format pixelformat)
{
  switch (pixelformat)
  {
    case RGBA5551:
    case RGBA8888:
    case RGBA4444:
    case BGRA8888:
    case BGRA4444:
    case BGRA5551:
    case A8:
    case LA88:
    // Note, Can be used for alpha if you want: COMPRESSED_R11_EAC,                       ///< ETC2 / EAC single-channel, unsigned
    // Note, Can be used for alpha if you want: COMPRESSED_SIGNED_R11_EAC,                ///< ETC2 / EAC single-channel, signed
    case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_RGBA8_ETC2_EAC:
    case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case ARGB2101010:
    case ABGR2101010:
    case RGBA1010102:
    case BGRA1010102:
    case AYUV:
    {
      return true;
    }

    case L8:
    case RGB565:
    case RGB888:
    case RGB8888: // alpha not used
    case BGR8888: // alpha not used
    case BGR565:
    case COMPRESSED_R11_EAC:
    case COMPRESSED_SIGNED_R11_EAC:
    case COMPRESSED_RG11_EAC:
    case COMPRESSED_SIGNED_RG11_EAC:
    case COMPRESSED_RGB8_ETC2:
    case COMPRESSED_SRGB8_ETC2:
    case COMPRESSED_RGB8_ETC1:
    case COMPRESSED_RGB_PVRTC_4BPPV1:
    case XRGB8888:
    case XBGR8888:
    case RGBX8888:
    case BGRX8888:
    case XRGB2101010:
    case XBGR2101010:
    case RGBX1010102:
    case BGRX1010102:
    case YUYV:
    case YVYU:
    case UYVY:
    case VYUY:
    case NV12:
    case NV21:
    case NV16:
    case NV61:
    case YUV410:
    case YVU410:
    case YUV411:
    case YVU411:
    case YUV420:
    case YVU420:
    case YUV422:
    case YVU422:
    case YUV444:
    case YVU444: 
    {
      return false;
    }
  }
  return false;
}

unsigned int Pixel::GetBytesPerPixel(Format pixelFormat)
{
  switch (pixelFormat)
  {
    case L8:
    case A8:
    {
      return 1;
    }

    case LA88:
    case RGB565:
    case RGBA4444:
    case RGBA5551:
    case BGR565:
    case BGRA4444:
    case BGRA5551:
    case YUYV:
    case YVYU:
    case UYVY:
    {
      return 2;
    }

    case RGB888:
    {
      return 3;
    }

    case RGB8888:
    case BGR8888:
    case RGBA8888:
    case BGRA8888:
    case XRGB8888:
    case XBGR8888:
    case RGBX8888:
    case BGRX8888:
    case XRGB2101010:
    case XBGR2101010:
    case RGBX1010102:
    case BGRX1010102:
    case ARGB2101010:
    case ABGR2101010:
    case RGBA1010102:
    case BGRA1010102:
    case AYUV:
    {
      return 4;
    }

    case COMPRESSED_R11_EAC:
    case COMPRESSED_SIGNED_R11_EAC:
    case COMPRESSED_RG11_EAC:
    case COMPRESSED_SIGNED_RG11_EAC:
    case COMPRESSED_RGB8_ETC2:
    case COMPRESSED_SRGB8_ETC2:
    case COMPRESSED_RGB8_ETC1:
    case COMPRESSED_RGB_PVRTC_4BPPV1:
    case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_RGBA8_ETC2_EAC:
    case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    {
      DALI_LOG_ERROR("Pixel formats for compressed images do not have meaningful integer bits per pixel values.\n");
      return 0;
    }

    case VYUY:
    case NV12:
    case NV21:
    case NV16:
    case NV61:
    case YUV410:
    case YVU410:
    case YUV411:
    case YVU411:
    case YUV420:
    case YVU420:
    case YUV422:
    case YVU422:
    case YUV444:
    case YVU444:
    {
      DALI_LOG_ERROR("This is YUV format. Bytes per one pixel should be ambiguous\n");
      return 0;
    }
  }
  return 0;
}

void Pixel::GetAlphaOffsetAndMask(Format pixelFormat, int& byteOffset, int& bitMask)
{
  switch (pixelFormat)
  {
    case A8:
    {
      byteOffset = 0;
      bitMask    = 0xFF;
    }
    break;

    case L8:
    case RGB888:
    case RGB565:
    case RGB8888:
    case BGR8888:
    case BGR565:
    case XRGB8888:
    case XBGR8888:
    case RGBX8888:
    case BGRX8888:
    case XRGB2101010:
    case XBGR2101010:
    case RGBX1010102:
    case BGRX1010102:
    {
      byteOffset=0;
      bitMask=0;
      break;
    }

    case LA88:
    {
      byteOffset=1;
      bitMask=0xff;
      break;
    }

    case RGBA4444:
    case BGRA4444:
    {
      byteOffset=1;
      bitMask=0x0f;
      break;
    }

    case RGBA5551:
    case BGRA5551:
    {
      byteOffset=1;
      bitMask=0x01;
      break;
    }

    case RGBA8888:
    case BGRA8888:
    {
      byteOffset=3;
      bitMask=0xff;
      break;
    }

    case ARGB2101010:
    case ABGR2101010:
    {
      byteOffset=0;
      bitMask=0x02;
      break;
    }
 
    case RGBA1010102:
    case BGRA1010102:
    {
      byteOffset=3;
      bitMask=0x02;
      break;
    }
    
    case AYUV:
    {
      byteOffset=0;
      bitMask=0xff;
      break;
    }

    case COMPRESSED_R11_EAC:
    case COMPRESSED_SIGNED_R11_EAC:
    case COMPRESSED_RG11_EAC:
    case COMPRESSED_SIGNED_RG11_EAC:
    case COMPRESSED_RGB8_ETC2:
    case COMPRESSED_SRGB8_ETC2:
    case COMPRESSED_RGB8_ETC1:
    case COMPRESSED_RGB_PVRTC_4BPPV1:
    case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_RGBA8_ETC2_EAC:
    case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    {
      DALI_LOG_ERROR("Pixel formats for compressed images are not compatible with simple masking-out of per-pixel alpha.\n");
      byteOffset=0;
      bitMask=0;
      break;
    }
    
    case YUYV:
    case YVYU:
    case UYVY:
    case VYUY:
    case NV12:
    case NV21:
    case NV16:
    case NV61:
    case YUV410:
    case YVU410:
    case YUV411:
    case YVU411:
    case YUV420:
    case YVU420:
    case YUV422:
    case YVU422:
    case YUV444:
    case YVU444:
    {
      byteOffset=0;
      bitMask=0;
      break;
    }
  }
}

} // namespace Dali
