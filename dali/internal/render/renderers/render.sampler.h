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

#include <dali/devel-api/rendering/sampler.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/resources/bitmap-metadata.h>
#include <dali/internal/render/data-providers/sampler-data-provider.h>

#include <string>

namespace Dali
{
namespace Internal
{
namespace Render
{
class Sampler
{
public:
  typedef Dali::Sampler::FilterMode FilterMode;
  typedef Dali::Sampler::WrapMode   WrapMode;

  /**
   * Constructor
   */
  Sampler()
  : mMinFilter( Dali::Sampler::DEFAULT ),
    mMagFilter( Dali::Sampler::DEFAULT ),
    mUWrapMode( Dali::Sampler::CLAMP_TO_EDGE ),
    mVWrapMode( Dali::Sampler::CLAMP_TO_EDGE )
  {}

  /**
   * Destructor
   */
  virtual ~Sampler(){}

  /**
   * Set the filter modes for minify and magnify filters
   * @param[in] bufferIndex The buffer index to use
   * @param[in] minFilter The minify filter
   * @param[in] magFilter The magnify filter
   */
  void SetFilterMode( BufferIndex bufferIndex, FilterMode minFilter, FilterMode magFilter )
  {}

  /**
   * @param[in] bufferIndex The buffer index to use
   */
  void SetWrapMode( BufferIndex bufferIndex, WrapMode uWrap, WrapMode vWrap )
  {}

public: // SamplerDataProvider interface - called from RenderThread

  /**
   * Get the filter mode
   * @param[in] bufferIndex The buffer index to use
   * @return The minify filter mode
   */
  virtual FilterMode GetMinifyFilterMode( BufferIndex bufferIndex ) const
  {
    return (FilterMode)mMinFilter;
  }

  /**
   * Get the filter mode
   * @param[in] bufferIndex The buffer index to use
   * @return The magnify filter mode
   */
  virtual FilterMode GetMagnifyFilterMode( BufferIndex bufferIndex ) const
  {
    return (FilterMode)mMagFilter;
  }

  /**
   * Get the horizontal wrap mode
   * @param[in] bufferIndex The buffer index to use
   * @return The horizontal wrap mode
   */
  virtual WrapMode GetUWrapMode( BufferIndex bufferIndex ) const
  {
    return (WrapMode)mUWrapMode;
  }

  /**
   * Get the vertical wrap mode
   * @param[in] bufferIndex The buffer index to use
   * @return The vertical wrap mode
   */
  virtual WrapMode GetVWrapMode( BufferIndex bufferIndex ) const
  {
    return (WrapMode)mVWrapMode;
  }

public:

  int  mMinFilter;    ///< The minify filter
  int  mMagFilter;    ///< The magnify filter
  int  mUWrapMode;    ///< The horizontal wrap mode
  int  mVWrapMode;    ///< The vertical wrap mode
};

} // namespace Render



} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_SAMPLER_H
