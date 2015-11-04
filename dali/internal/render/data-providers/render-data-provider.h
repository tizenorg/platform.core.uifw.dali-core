#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H__

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
 *
 */

#include <dali/internal/render/data-providers/material-data-provider.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/property-buffer-data-provider.h>
#include <dali/internal/render/data-providers/uniform-map-data-provider.h>
#include <dali/internal/render/renderers/render-texture.h>
namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class PropertyBuffer;
class Shader;

/**
 * Wraps all the data providers for the renderer. This allows the
 * individual providers to change connections in the update thread without affecting
 * the current render. It essentially provides double buffering of the
 * provider accessor through the message system. It does not take ownership
 * of any objects.
 *
 * @todo - Add set of bools to state what changed (For improving performance)
 */
class RenderDataProvider
{
public:
  typedef Dali::Vector< const PropertyBufferDataProvider* > VertexBuffers;
  typedef std::vector< Render::Texture > Textures;

  /**
   * Constructor.
   * The RendererAttachment that creates this object will initialize the members
   * directly.
   */
  RenderDataProvider();

  /**
   * Destructor
   */
  ~RenderDataProvider();

public:

  /**
   * Set the material data provider
   * @param[in] materialDataProvider The material data provider
   */
  void SetMaterial( const MaterialDataProvider& materialDataProvider );
  void SetBatchMaterial( const MaterialDataProvider& materialDataProvider );

  /**
   * Get the material data provider
   * @return the material data provider
   */
  const MaterialDataProvider& GetMaterial() const;

  const MaterialDataProvider* GetBatchMaterial() const;

  /**
   * Set the uniform map data provider
   * @param[in] uniformMapDataProvider The uniform map data provider
   */
  void SetUniformMap(const UniformMapDataProvider& uniformMapDataProvider);

  /**
   * Get the uniform map data provider
   */
  const UniformMapDataProvider& GetUniformMap() const;

  /**
   * Set the shader data provider
   * @param[in] shader The shader data provider
   */
  void SetShader( Shader& shader );

  /**
   * Returns the shader
   * @return The shader
   */
  Shader& GetShader() const;

  /**
   * Returns the list of textures
   * @return The list of textures
   */
  Textures& GetTextures();

//private:
  const MaterialDataProvider*   mMaterialDataProvider;
  const MaterialDataProvider*   mBatchMaterialDataProvider;
  const UniformMapDataProvider* mUniformMapDataProvider;
  Shader*                       mShader;
  Textures                      mTextures;

// Give Renderer access to our private data to reduce copying vectors on construction.
  friend class Renderer;
};

} // SceneGraph
} // Internal
} // Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_DATA_PROVIDER_H__
