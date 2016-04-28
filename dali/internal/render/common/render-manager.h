#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_MANAGER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_MANAGER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/common/texture-uploaded-dispatcher.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>

namespace Dali
{

namespace Integration
{
class GlAbstraction;
class GlSyncAbstraction;
class RenderStatus;
}

struct Vector4;

namespace Internal
{
class Context;
class ProgramCache;
class ShaderSaver;

namespace Render
{
class Renderer;
class Sampler;
class RenderTracker;
class Geometry;
}

namespace SceneGraph
{
class RenderQueue;
class TextureCache;
class RenderInstruction;
class RenderInstructionContainer;
class Shader;
class PropertyBufferDataProvider;

/**
 * RenderManager is responsible for rendering the result of the previous "update", which
 * is provided in a RenderCommand during UpdateManager::Update().
 */
class RenderManager : public TextureUploadedDispatcher
{
public:

  /**
   * Construct a new RenderManager.
   * @param[in]  glAbstraction The GL abstraction used for rendering.
   * @param[in]  glSyncAbstraction The GL sync abstraction used fence sync creation/deletion.
   * @param[out] resourcePostProcessQueue A queue for sending rendered texture ids to the update-thread.
   */
  static RenderManager* New( Integration::GlAbstraction& glAbstraction,
                             Integration::GlSyncAbstraction& glSyncAbstraction,
                             LockedResourceQueue& resourcePostProcessQueue );

  /**
   * Non-virtual destructor; not intended as a base class
   */
  ~RenderManager();

  /**
   * Retrieve the RenderQueue. Messages should only be added to this from the update-thread.
   * @return The render queue.
   */
  RenderQueue& GetRenderQueue();

  /**
   * Retrieve the texture cache. Messages should only be sent to this from the update thread,
   * accessor methods should only be used from the render thread.
   * @return The texture cache
   */
  TextureCache& GetTextureCache();

  /**
   * @copydoc Dali::Integration::Core::ContextCreated()
   */
  void ContextCreated();

  /**
   * @copydoc Dali::Integration::Core::ContextToBeDestroyed()
   */
  void ContextDestroyed();

  /**
   * Dispatch requests onto the postProcessResourcesQueue
   * @param[in] resource The Id of the resource to dispatch
   */
  virtual void DispatchTextureUploaded( ResourceId resource );

  /**
   * Set the upstream interface for compiled shader binaries to be sent back to for eventual
   * caching and saving.
   * @param[in] upstream The abstract interface to send any received ShaderDatas onwards to..
   * @note This should be called during core initialisation if shader binaries are to be used.
   */
  void SetShaderSaver( ShaderSaver& upstream );

  /**
   * Retrieve the render instructions; these should be set during each "update" traversal.
   * @return The render instruction container.
   */
  RenderInstructionContainer& GetRenderInstructionContainer();

  // The following methods should be called via RenderQueue messages

  /**
   * Set the background color i.e. the glClear color used at the beginning of each frame.
   * @param[in] color The new background color.
   */
  void SetBackgroundColor( const Vector4& color );

  /*
   * Set the frame time delta (time elapsed since the last frame.
   * @param[in] deltaTime the delta time
   */
  void SetFrameDeltaTime( float deltaTime );

  /**
   * Returns the rectangle for the default surface (probably the application window).
   * @return Rectangle for the surface.
   */
  void SetDefaultSurfaceRect( const Rect<int>& rect );

  /**
   * Add a Renderer to the render manager.
   * @param[in] renderer The renderer to add.
   * @post renderer is owned by RenderManager
   */
  void AddRenderer( Render::Renderer* renderer );

  /**
   * Remove a Renderer from the render manager.
   * @param[in] renderer The renderer to remove.
   * @post renderer is destroyed.
   */
  void RemoveRenderer( Render::Renderer* renderer );

  /**
   * Add a sampler to the render manager.
   * @param[in] sampler The sampler to add.
   * @post sampler is owned by RenderManager
   */
  void AddSampler( Render::Sampler* sampler );

  /**
   * Remove a sampler from the render manager.
   * @param[in] sampler The sampler to remove.
   * @post sampler is destroyed.
   */
  void RemoveSampler( Render::Sampler* sampler );

  /**
   * Set minification and magnification filter modes for a sampler
   * @param[in] minFilterMode Filter mode to use when the texture is minificated
   * @param[in] magFilterMode Filter mode to use when the texture is magnified
   */
  void SetFilterMode( Render::Sampler* sampler, unsigned int minFilterMode, unsigned int magFilterMode );

  /**
   * Set wrapping mode for a sampler
   * @param[in] uWrapMode Wrap mode in the x direction
   * @param[in] vWrapMode Wrap mode in the y direction
   */
  void SetWrapMode( Render::Sampler* sampler, unsigned int uWrapMode, unsigned int vWrapMode );

  /**
   * Add a property buffer to the render manager.
   * @param[in] propertyBuffer The property buffer to add.
   * @post propertBuffer is owned by RenderManager
   */
  void AddPropertyBuffer( Render::PropertyBuffer* propertyBuffer );

  /**
   * Remove a property buffer from the render manager.
   * @param[in] propertyBuffer The property buffer to remove.
   * @post propertyBuffer is destroyed.
   */
  void RemovePropertyBuffer( Render::PropertyBuffer* propertyBuffer );

  /**
   * Add a geometry to the render manager.
   * @param[in] geometry The geometry to add.
   * @post geometry is owned by RenderManager
   */
  void AddGeometry( Render::Geometry* geometry );

  /**
   * Remove a geometry from the render manager.
   * @param[in] geometry The geometry to remove.
   * @post geometry is destroyed.
   */
  void RemoveGeometry( Render::Geometry* geometry );

  /**
   * Adds a property buffer to a geometry from the render manager.
   * @param[in] geometry The geometry
   * @param[in] propertyBuffer The property buffer to remove.
   */
  void AddVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer );

  /**
   * Remove a property buffer from a Render::Geometry from the render manager.
   * @param[in] geometry The geometry
   * @param[in] propertyBuffer The property buffer to remove.
   * @post property buffer is destroyed.
   */
  void RemoveVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer );

  /**
   * Sets the format of an existing property buffer
   * @param[in] propertyBuffer The property buffer.
   * @param[in] format The new format of the buffer
   */
  void SetPropertyBufferFormat( Render::PropertyBuffer* propertyBuffer, Render::PropertyBuffer::Format* format );

  /**
   * Sets the data of an existing property buffer
   * @param[in] propertyBuffer The property buffer.
   * @param[in] data The new data of the buffer
   * @param[in] size The new size of the buffer
   */
  void SetPropertyBufferData( Render::PropertyBuffer* propertyBuffer, Dali::Vector<char>* data, size_t size );

  /**
   * Sets the data for the index buffer of an existing geometry
   * @param[in] geometry The geometry
   * @param[in] data A vector containing the indices
   */
  void SetIndexBuffer( Render::Geometry* geometry, Dali::Vector<unsigned short>& data );

  /**
   * Set the geometry type of an existing render geometry
   * @param[in] geometry The render geometry
   * @param[in] geometryType The new geometry type
   */
  void SetGeometryType( Render::Geometry* geometry, unsigned int geometryType );

  /**
   * Adds a render tracker to the RenderManager. RenderManager takes ownership of the
   * tracker. The lifetime of the tracker is related to the lifetime of the tracked
   * object, usually an offscreen render task.
   * @param[in] renderTracker The render tracker
   */
  void AddRenderTracker( Render::RenderTracker* renderTracker );

  /**
   * Removes a render tracker from the RenderManager.
   * @param[in] renderTracker The render tracker to remove.
   */
  void RemoveRenderTracker( Render::RenderTracker* renderTracker );

  /**
   * Set the default shader that is to be used in absence of custom shader
   * @param[in] shader that is the default one
   */
  void SetDefaultShader( Shader* shader );

  /**
   * returns the Program controller for sending program messages
   * @return the ProgramController
   */
  ProgramCache* GetProgramCache();

  // This method should be called from Core::Render()

  /**
   * Renders the results of the previous "update" traversal.
   * @param[out] status contains the flag that indicates if render instructions exist
   * @return true if a further update is required
   */
  bool Render( Integration::RenderStatus& status );

private:

  /**
   * Helper to process a single RenderInstruction.
   * @param[in] instruction A description of the rendering operation.
   * @param[in] defaultShader default shader to use.
   */
  void DoRender( RenderInstruction& instruction, Shader& defaultShader );

private:

  /**
   * Construct a new RenderManager.
   */
  RenderManager();

  // Undefined
  RenderManager( const RenderManager& );

  // Undefined
  RenderManager& operator=( const RenderManager& rhs );

private:

  struct Impl;
  Impl* mImpl;

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_MANAGER_H__
