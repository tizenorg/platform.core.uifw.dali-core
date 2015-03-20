#ifndef __DALI_IMAGE_OPERATIONS_H__
#define __DALI_IMAGE_OPERATIONS_H__

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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/math/vector2-uint-16.h>

namespace Dali
{

/**
 * @brief The integer dimensions of an image or a region of an image packed into
 *        16 bits per component.
 *
 * This can only be used for images of up to 65535 x 65535 pixels.
 */
typedef Vector2Uint16 ImageDimensions;

/**
 * @brief Scaling options, used when resizing images to fit desired dimensions.
 *
 * A scaling mode controls the region of a loaded image to be mapped to the
 * desired image rectangle specified using ImageAttributes.SetSize().
 * All scaling modes preserve the aspect ratio of the image contents.
 * @note Store member variables as ScalingModeEnum rather than ScalingMode to
 * avoid paying for class alignment.
 */
struct ScalingMode
{
  enum ScalingModeEnum
  {
    ShrinkToFit, ///< Fit full image inside desired width & height, potentially not filling one of either the desired image width or height with pixels.
    ScaleToFill, ///< Image fills whole desired width & height with image data. The image is centred in the desired dimensions, exactly touching in one dimension, with image regions outside the other desired dimension cropped away.
    FitWidth,    ///< Image fills whole width. Height is scaled proportionately to maintain aspect ratio.
    FitHeight    ///< Image fills whole height. Width is scaled proportionately to maintain aspect ratio.
  };
  // Helpers for passing this around:
  ScalingMode( ScalingModeEnum mode = ShrinkToFit ) : mEnum( mode ) {}
  operator ScalingModeEnum() const { return mEnum; }
  ScalingModeEnum mEnum;
};

/**
 * @brief Filtering options, used when resizing images on load to sample original pixels.
 *
 * A SamplingMode controls how pixels in an input image are sampled and
 * combined to generate each pixel of a destination image during a scaling.
 *
 * NoFilter and Box modes do not guarantee that the loaded pixel array
 * exactly matches the rectangle specified by the desired dimensions and
 * ScalingMode, but all other filter modes do if the desired dimensions are
 * `<=` the raw dimensions of the image file.
 * @note Store member variables as SamplingModeEnum rather than SamplingMode to
 * avoid paying for class alignment.
 */
struct SamplingMode
{

  enum SamplingModeEnum
  {
    Box,            ///< Iteratively box filter to generate an image of 1/2, 1/4, 1/8, ... width and height and
                    ///  approximately the desired size. If the ScaleToFill scaling mode is enabled, cut away the
                    ///  top/bottom or left/right borders of the image to match the aspect ratio of desired dimensions.
                    ///  This is the default.
    Nearest,        ///< For each output pixel, read one input pixel.
    Linear,         ///< For each output pixel, read a quad of four input pixels and write a weighted average of them.
    BoxThenNearest, ///< Iteratively box filter to generate an image of 1/2, 1/4, 1/8, ... width and height and
                    ///  approximately the desired size, then for each output pixel, read one pixel from the last level
                    ///  of box filtering.
    BoxThenLinear,  ///< Iteratively box filter to almost the right size, then for each output pixel, read four pixels
                    ///  from the last level of box filtering and write their weighted average.
    NoFilter,       ///< No filtering is performed. If the ScaleToFill scaling mode is enabled, the borders of the
                    ///  image may be trimmed to match the aspect ratio of the desired dimensions.
    DontCare        ///< For when the client strongly prefers a cache-hit to reuse a previously loaded image.
                    ///  If the cache misses, the loading of the image defaults to Box.
  };
  // Helpers for passing this around:
  SamplingMode( SamplingModeEnum mode = Box ) : mEnum( mode ) {}
  operator SamplingModeEnum() const { return mEnum; }
  SamplingModeEnum mEnum;
};

/**
 * * @brief Scaling options, used when resizing images to fit desired dimensions.
 */
namespace Scaling /// ImageScaling?
{
/**
 * @brief Policy used when resizing images to fit desired dimensions.
 *
 * A scaling mode controls the region of a loaded image to be mapped to the
 * desired image rectangle specified using ImageAttributes.SetSize().
 * All scaling modes preserve the aspect ratio of the image contents.
 * @note Store member variables as ScalingModeEnum rather than ScalingMode to
 * avoid paying for class alignment.
 */
enum FittingMode
{
  FITTING_MODE_SHRINK_TO_FIT,
  FITTING_MODE_SCALE_TO_FILL,
    ShrinkToFit, ///< Fit full image inside desired width & height, potentially not filling one of either the desired image width or height with pixels.
    ScaleToFill, ///< Image fills whole desired width & height with image data. The image is centred in the desired dimensions, exactly touching in one dimension, with image regions outside the other desired dimension cropped away.
    FitWidth,    ///< Image fills whole width. Height is scaled proportionately to maintain aspect ratio.
    FitHeight    ///< Image fills whole height. Width is scaled proportionately to maintain aspect ratio.
};

/**
 * @brief Filtering options, used when resizing images on load to sample original pixels.
 *
 * A SamplingMode controls how pixels in an input image are sampled and
 * combined to generate each pixel of a destination image during a scaling.
 *
 * NoFilter and Box modes do not guarantee that the loaded pixel array
 * exactly matches the rectangle specified by the desired dimensions and
 * ScalingMode, but all other filter modes do if the desired dimensions are
 * `<=` the raw dimensions of the image file.
 * @note Store member variables as SamplingModeEnum rather than SamplingMode to
 * avoid paying for class alignment.
 */
enum SamplingMode
{
  SAMPLING_MODE_BOX,
  SAMPLING_MODE_NEAREST,
  Box,            ///< Iteratively box filter to generate an image of 1/2, 1/4, 1/8, ... width and height and
                  ///  approximately the desired size. If the ScaleToFill scaling mode is enabled, cut away the
                  ///  top/bottom or left/right borders of the image to match the aspect ratio of desired dimensions.
                  ///  This is the default.
  Nearest,        ///< For each output pixel, read one input pixel.
  Linear,         ///< For each output pixel, read a quad of four input pixels and write a weighted average of them.
  BoxThenNearest, ///< Iteratively box filter to generate an image of 1/2, 1/4, 1/8, ... width and height and
                  ///  approximately the desired size, then for each output pixel, read one pixel from the last level
                  ///  of box filtering.
  BoxThenLinear,  ///< Iteratively box filter to almost the right size, then for each output pixel, read four pixels
                  ///  from the last level of box filtering and write their weighted average.
  NoFilter,       ///< No filtering is performed. If the ScaleToFill scaling mode is enabled, the borders of the
                  ///  image may be trimmed to match the aspect ratio of the desired dimensions.
  DontCare        ///< For when the client strongly prefers a cache-hit to reuse a previously loaded image.
                  ///  If the cache misses, the loading of the image defaults to Box.
};

inline void usage()
{
  FittingMode fittingMode = FITTING_MODE_SHRINK_TO_FIT;
  SamplingMode samplingMode = SAMPLING_MODE_BOX;
  DALI_ASSERT_ALWAYS( (void*)&fittingMode != (void*)&samplingMode );
}

} // namespace Scaling


namespace ScalingOption2 /// ImageScaling?
{

namespace FittingMode
{
  enum Type
  {
    SHRINK_TO_FIT,
    SCALE_TO_FILL
  };
}

namespace SamplingMode
{
  enum Type
  {
    BOX,
    NEAREST
  };
}

inline void usage()
{
  FittingMode::Type fittingMode = FittingMode::SHRINK_TO_FIT;
  SamplingMode::Type samplingMode = SamplingMode::BOX;
  DALI_ASSERT_ALWAYS( (void*)&fittingMode != (void*)&samplingMode );
}

}

} // namespace Dali

#endif // __DALI_IMAGE_OPERATIONS_H__
