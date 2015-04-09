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
#include <dali/internal/event/resources/resource-type-path.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * @brief Compare two sets of image loading parameters for equality.
 */
inline bool AttributesEqual( ImageDimensions aDims, ScalingMode aScaling, SamplingMode aSampling, bool aOrient, ImageDimensions bDims, ScalingMode bScaling, SamplingMode bSampling, bool bOrient )
{
  return aDims     == bDims &&
         aScaling  == bScaling &&
         aSampling == bSampling &&
         aOrient   == bOrient;
}

/**
 * @brief Compare two sets of image loading parameters
 * @pre The two sets are not identical.
 */
inline bool AttributesLessAssumingNotEqual( ImageDimensions aDims, ScalingMode aScaling, SamplingMode aSampling, bool aOrient, ImageDimensions bDims, ScalingMode bScaling, SamplingMode bSampling, bool bOrient )
{
  return aDims     < bDims &&
         aScaling  < bScaling &&
         aSampling < bSampling &&
         aOrient   < bOrient;
}

/**
 * Compare two resource types.
 * @return zero if the types are equal,
 *         less than zero if lhs < rhs or
 *         greater than zero if lhs > rhs
 */
int ResourceTypeCompare(const ResourceType& lhs, const ResourceType& rhs)
{
  int result = 0;

  if (lhs.id != rhs.id)
  {
    result = lhs.id > rhs.id ? 1 : -1;
  }
  else // lhs.id == rhs.id
  {
    // Cast to a derived resource type and compare...

    switch (lhs.id)
    {
      case ResourceBitmap:
      {
        // compare bitmap widths & heights
        const BitmapResourceType& lhsBitmap = static_cast<const BitmapResourceType&>(lhs);
        const BitmapResourceType& rhsBitmap = static_cast<const BitmapResourceType&>(rhs);

        if( ! AttributesEqual( lhsBitmap.size, lhsBitmap.scalingMode, lhsBitmap.samplingMode, lhsBitmap.orientationCorrection,
                               rhsBitmap.size, rhsBitmap.scalingMode, rhsBitmap.samplingMode, rhsBitmap.orientationCorrection ) )
        {
          result = AttributesLessAssumingNotEqual( lhsBitmap.size, lhsBitmap.scalingMode, lhsBitmap.samplingMode, lhsBitmap.orientationCorrection,
                                                   rhsBitmap.size, rhsBitmap.scalingMode, rhsBitmap.samplingMode, rhsBitmap.orientationCorrection );
        }
        // else result = 0
        break;
      }

      case ResourceNativeImage:
      {
        // compare native image widths & heights

        const NativeImageResourceType& lhsNativeImage = static_cast<const NativeImageResourceType&>(lhs);
        const NativeImageResourceType& rhsNativeImage = static_cast<const NativeImageResourceType&>(rhs);

        if (lhsNativeImage.imageDimensions != rhsNativeImage.imageDimensions)
        {
          result = lhsNativeImage.imageDimensions < rhsNativeImage.imageDimensions ? -1 : 1;
        }
        // else result = 0
        break;
      }

      case ResourceTargetImage:
      {
        // compare bitmap widths & heights
        const RenderTargetResourceType& lhsImage = static_cast<const RenderTargetResourceType&>(lhs);
        const RenderTargetResourceType& rhsImage = static_cast<const RenderTargetResourceType&>(rhs);

        if (lhsImage.imageDimensions != rhsImage.imageDimensions)
        {
          result = lhsImage.imageDimensions < rhsImage.imageDimensions ? -1 : 1;
        }
        // else result = 0
        break;
      }

      case ResourceShader:
      {
        // compare shader source hashes
        const ShaderResourceType& lhsShader = static_cast<const ShaderResourceType&>(lhs);
        const ShaderResourceType& rhsShader = static_cast<const ShaderResourceType&>(rhs);

        if (lhsShader.hash != rhsShader.hash)
        {
          result = lhsShader.hash < rhsShader.hash ? -1 : 1;
        }
        // else result = 0
        break;
      }


      case ResourceText:
      {
        // compare text requests
        const TextResourceType& lhsText = static_cast<const TextResourceType&>(lhs);
        const TextResourceType& rhsText = static_cast<const TextResourceType&>(rhs);

        if( lhsText.mStyle != rhsText.mStyle )
        {
          result = lhsText.mStyle < rhsText.mStyle ? -1 : 1;
        }
        else if (lhsText.mCharacterList.size() != rhsText.mCharacterList.size())
        {
          result = lhsText.mCharacterList.size() < rhsText.mCharacterList.size() ? -1 : 1;
        }
        else if (!std::equal(lhsText.mCharacterList.begin(), lhsText.mCharacterList.end(), rhsText.mCharacterList.begin()))
        {
          for (unsigned int i = 0; i < lhsText.mCharacterList.size(); ++i)
          {
            if (lhsText.mCharacterList[i].character != rhsText.mCharacterList[i].character)
            {
              result = lhsText.mCharacterList[i].character <= rhsText.mCharacterList[i].character ? -1 : 1;
              break;
            }
          }
        }
        else if (lhsText.mFontHash != rhsText.mFontHash)
        {
          result = lhsText.mFontHash < rhsText.mFontHash ? -1 : 1;
        }
        else if (lhsText.mQuality != rhsText.mQuality)
        {
          result = lhsText.mQuality < rhsText.mQuality ? -1 : 1;
        }
        break;
      }

      case ResourceMesh:
      {
        break; // result = 0
      }

    }
  }
  return result;
}
}

bool ResourceTypePath::operator<(const ResourceTypePath& rhs) const
{
  int typeComparison = ResourceTypeCompare(*type, *(rhs.type));

  if (typeComparison != 0)
  {
    return (typeComparison < 0);
  }
  else
  {
    return path < rhs.path;
  }
}

} // namespace Internal

} // namespace Dali
