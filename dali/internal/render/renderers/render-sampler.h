#ifndef DALI_INTERNAL_RENDER_SAMPLER_H
#define DALI_INTERNAL_RENDER_SAMPLER_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/actors/sampling.h>
#include <dali/devel-api/rendering/sampler.h>
#include <dali/internal/render/renderers/render-sampler.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

struct Sampler
{

  typedef Dali::FilterMode::Type FilterMode;
  typedef Dali::WrapMode::Type   WrapMode;

  /**
   * Constructor
   */
  Sampler()
  :mMinificationFilter(FilterMode::DEFAULT),
   mMagnificationFilter(FilterMode::DEFAULT),
   mSWrapMode(WrapMode::DEFAULT),
   mTWrapMode(WrapMode::DEFAULT),
   mUWrapMode(WrapMode::DEFAULT)
  {}

  /**
   * Destructor
   */
  ~Sampler()
  {}


  union
  {
    unsigned int mBitfield;

    struct
    {
      FilterMode  mMinificationFilter   : 2;    ///< The minify filter
      FilterMode  mMagnificationFilter  : 2;    ///< The magnify filter
      WrapMode    mSWrapMode            : 2;    ///< The horizontal wrap mode
      WrapMode    mTWrapMode            : 2;    ///< The vertical wrap mode
      WrapMode    mUWrapMode            : 2;    ///< The vertical wrap mode
    };

  };
};

} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_SAMPLER_H
