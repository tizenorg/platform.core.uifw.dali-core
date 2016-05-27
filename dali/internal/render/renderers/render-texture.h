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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/sampler.h>
#include <dali/devel-api/rendering/texture.h>
#include <dali/integration-api/resource-declarations.h>

#include <dali/internal/render/gl-resources/context.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <stdio.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class Sampler;

/**
 * This class is the mapping between texture id, sampler and sampler uniform name
 */
class Texture
{
public:

  /**
   * Enumeration to tell that this sampler does not have a unique index yet
   */
  enum
  {
    NOT_INITIALIZED = -1
  };

  /**
   * Constructor
   */
  Texture()
  : mSampler( 0 ),
    mTextureId( Integration::InvalidResourceId )
  {}

  /**
   * Constructor
   */
  Texture( Integration::ResourceId textureId, Render::Sampler* sampler )
  : mSampler( sampler ),
    mTextureId( textureId)
  {}

  /**
   * Destructor
   */
  ~Texture()
  {}

  /*
   * Get the Render::Sampler used by the texture
   * @Return The Render::Sampler being used or 0 if using the default
   */
  inline const Render::Sampler* GetSampler() const
  {
    return mSampler;
  }

public: // called from RenderThread

  /**
   * Get the texture ID
   * @return the id of the associated texture
   */
  inline Integration::ResourceId GetTextureId() const
  {
    return mTextureId;
  }

private:

  Render::Sampler*        mSampler;
  Integration::ResourceId mTextureId;
};


class NewTexture
{
public:
  NewTexture( Dali::TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height, bool mipmaps );

  void Initialize(Context& context);
  void Upload( );
  void Bind( Context& context, unsigned int textureUnit, Render::Sampler* sampler );
  bool HasAlphaChannel();

private:

  void ApplySampler( Context& context, Render::Sampler* sampler );

  GLuint mId;
  Dali::TextureType::Type mType;
  Pixel::Format mFormat;
  unsigned int mWidth;
  unsigned int mHeight;
  Render::Sampler mSampler;
  unsigned int mMipmapCount;
};


} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_TEXTURE_H
