#ifndef DALI_INTERNAL_RENDER_TEXTURE_H
#define DALI_INTERNAL_RENDER_TEXTURE_H

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
class Sampler;

class Texture
{
public:

  /**
   * Constructor
   */
  Texture()
  :mUniformName(""),
   mTextureId(Integration::InvalidResourceId),
   mSampler(0)
  {}

  /**
   * Constructor
   */
  Texture( const std::string& samplerName, Integration::ResourceId textureId, Render::Sampler* sampler )
  :mUniformName(samplerName),
   mTextureId( textureId),
   mSampler(sampler)
  {}

  /**
   * Destructor
   */
  virtual ~Texture()
  {}

  /**
   * Set the uniform name of this sampler. This allows the shader to find the
   * GL index of this sampler.
   */
  void SetUniformName( const std::string& uniformName )
  {
    mUniformName = uniformName;
  }

  /**
   * Set the texture identity of this sampler (needs to double buffer this value because
   * it can be read through the data provider interface in the render thread )
   * @param[in] bufferIndex The buffer index to use
   * @param[in] textureId The identity of the texture
   */
  void SetTextureId( Integration::ResourceId textureId )
  {
    mTextureId = textureId;
  }

  void SetSampler( Render::Sampler* sampler )
  {
    mSampler = sampler;
  }


public: // SamplerDataProvider interface - called from RenderThread
  /**
   * Get the texture unit uniform name
   * @return the name of the texture unit uniform
   */
  virtual const std::string& GetUniformName() const
  {
    return mUniformName;
  }

  /**
   * Get the texture ID
   * @param[in] bufferIndex The buffer index to use
   * @return the identity of the associated texture
   */
  virtual Integration::ResourceId GetTextureId() const
  {
    return mTextureId;
  }



public:

private:
  std::string               mUniformName;
  Integration::ResourceId   mTextureId;
  Render::Sampler*          mSampler;
};

} // namespace SceneGraph

} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_TEXTURE_H
