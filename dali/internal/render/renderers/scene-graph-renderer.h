#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDERER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDERER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector4.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/message.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/frustum.h>
#include <dali/internal/render/renderers/scene-graph-renderer-declarations.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/type-abstraction-enums.h>

namespace Dali
{

namespace Internal
{
class Context;
class Texture;
class Program;

namespace SceneGraph
{
class SceneController;
class Shader;
class TextureCache;
class RenderDataProvider;

/**
 * Renderers are used to render images, text, & meshes etc.
 * These objects are used during RenderManager::Render(), so properties modified during
 * the Update must either be double-buffered, or set via a message added to the RenderQueue.
 */
class Renderer : public GlResourceOwner
{
public:

  /**
   * Second-phase construction.
   * This is called when the renderer is inside render thread
   * @param[in] context to use
   * @param[in] textureCache to use
   */
  void Initialize( Context& context, TextureCache& textureCache );

  /**
   * Virtual destructor
   */
  virtual ~Renderer();

  /**
   * Set the Shader used to render.
   * @param[in] shader The shader used to render.
   */
  void SetShader( Shader* shader );

  /**
   * Set whether the ImageRenderer should use blending
   * @param[in] useBlend True if blending should be used.
   */
  void SetUseBlend( bool useBlend );

  /**
   * Set the blending options.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions( unsigned int options );

  /**
   * Set the blend color.
   * @param[in] color The new blend-color.
   */
  void SetBlendColor( const Vector4& color );

  /**
   * Set the face-culling mode.
   * @param[in] mode The face-culling mode.
   */
  void SetCullFace( CullFaceMode mode );

  /**
   * Query whether the derived type of Renderer requires depth testing.
   * @return True if the renderer requires depth testing.
   */
  virtual bool RequiresDepthTest() const = 0;

  /**
   * Called to render during RenderManager::Render().
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] modelViewMatrix The model-view matrix.
   * @param[in] viewMatrix The view matrix.
   * @param[in] projectionMatrix The projection matrix.
   * @param[in] frametime The elapsed time between the last two updates.
   * @param[in] frustum The frustum planes in model space
   */
  void Render( BufferIndex bufferIndex,
               const Matrix& modelViewMatrix,
               const Matrix& viewMatrix,
               const Matrix& projectionMatrix,
               float frametime,
               const Frustum* frustum );

protected:

  /**
   * Protected constructor; only derived classes can be instantiated.
   * @param dataprovider for rendering
   */
  Renderer( RenderDataProvider& dataprovider );

private:

  // Undefined
  Renderer( const Renderer& );

  // Undefined
  Renderer& operator=( const Renderer& rhs );

  /**
   * Checks if renderer's resources are ready to be used.
   *
   * @return \e true if they are. Otherwise \e false.
   */
  virtual bool CheckResources() = 0;

  /**
   * Resolve the derived renderers geometry type and subtype
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[out] outType    The geometry type
   * @param[out] outSubType The geometry subtype
   */
  virtual void ResolveGeometryTypes( BufferIndex bufferIndex, GeometryType& outType, ShaderSubTypes& outSubType ) = 0;

  /**
   * Checks if renderer is culled.
   * @param[in] frustum The view frustum plane equations
   * @param[in] modelMatrix The model matrix.
   * @param[in] modelViewProjectionMatrix The MVP matrix.
   * @return \e true if it is. Otherwise \e false.
   */
  virtual bool IsOutsideClipSpace( const Frustum& frustum, const Matrix& modelMatrix, const Matrix& modelViewProjectionMatrix ) = 0;

  /**
   * Called from Render; implemented in derived classes.
   * @param[in] bufferIndex The index of the previous update buffer.
   * @param[in] program to use.
   * @param[in] modelViewMatrix The model-view matrix.
   * @param[in] viewMatrix The view matrix.
   */
  virtual void DoRender( BufferIndex bufferIndex, Program& program, const Matrix& modelViewMatrix, const Matrix& viewMatrix ) = 0;

protected:

  RenderDataProvider& mDataProvider;
  Context* mContext;
  TextureCache* mTextureCache;
  Shader* mShader;

private:

  BlendingOptions mBlendingOptions;
  bool mUseBlend:1;                 ///< True if blending should be enabled, 1 bit is enough
  CullFaceMode mCullFaceMode:3;     ///< cullface enum, 3 bits is enough
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDERER_H__
