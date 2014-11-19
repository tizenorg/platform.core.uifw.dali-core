#ifndef DALI_INTERNAL_TEXTURE_RECYCLING_CONFIGURATION_H
#define DALI_INTERNAL_TEXTURE_RECYCLING_CONFIGURATION_H
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
 */

#include <dali/public-api/images/pixel.h>

namespace Dali
{
namespace Internal
{
const unsigned int  DEFAULT_MAX_NUMBER_OF_RECYCLABLE_TEXTURES(10u);
const unsigned int  DEFAULT_WIDTH_OF_RECYCLABLE_TEXTURES(100u);
const unsigned int  DEFAULT_HEIGHT_OF_RECYCLABLE_TEXTURES(100u);
const Pixel::Format DEFAULT_FORMAT_OF_RECYCLABLE_TEXTURES(Pixel::RGB888);

/**
 * Configuration information for recycling textures
 */
struct TextureRecyclingConfiguration
{
  TextureRecyclingConfiguration()
  : maxNumberOfTextures(DEFAULT_MAX_NUMBER_OF_RECYCLABLE_TEXTURES),
    width(DEFAULT_WIDTH_OF_RECYCLABLE_TEXTURES),
    height(DEFAULT_HEIGHT_OF_RECYCLABLE_TEXTURES),
    pixelFormat(DEFAULT_FORMAT_OF_RECYCLABLE_TEXTURES)
  {
  }

  TextureRecyclingConfiguration( unsigned int theMaxNumberOfTextures,
                                 unsigned int theWidth,
                                 unsigned int theHeight,
                                 Pixel::Format thePixelFormat )
  : maxNumberOfTextures(theMaxNumberOfTextures),
    width(theWidth),
    height(theHeight),
    pixelFormat(thePixelFormat)
  {
  }

  unsigned int maxNumberOfTextures; ///< The maximum number of bitmap textures to recycle
  unsigned int width;    ///< The width of bitmaps to recycle
  unsigned int height;   ///< The height of bitmaps to recycle
  Pixel::Format pixelFormat;  ///< The pixel format of bitmaps to recycle
};

} // namespace Internal
} // namespace Dali


#endif //DALI_INTERNAL_TEXTURE_RECYCLING_CONFIGURATION_H
