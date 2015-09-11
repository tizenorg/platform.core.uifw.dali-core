#ifndef DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H
#define DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H

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

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/render/data-providers/material-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Sampler;
class Shader;
class ConnectionObserver;
class SceneController;

class Material : public PropertyOwner, public MaterialDataProvider, public UniformMap::Observer, public ConnectionChangePropagator::Observer
{
public:
  /**
   * This enum defines the outputs of the PrepareRender step, and is used
   * by the Renderer to determine final opacity.
   */
  enum BlendPolicy
  {
    OPAQUE,          ///< If the renderer should always be opaque
    TRANSPARENT,     ///< If the renderer should always be transparent
    USE_ACTOR_COLOR  ///< If the renderer should determine opacity using the actor color
  };

  /**
   * Constructor
   */
  Material();

  /**
   * Destructor
   */
  virtual ~Material();

  /**
   * Set the shader effect for this material
   * @param[in] shader The shader effect to use
   */
  void SetShader( Shader* shader );

  /**
   * Add a sampler (image + sampler modes) to the material
   * @param[in] sampler A sampler to add
   */
  void AddSampler( Sampler* sampler );

  /**
   * Remove a sampler (image + sampler modes) from the material
   * @param[in] sampler A sampler to remove
   */
  void RemoveSampler( Sampler* sampler );

  /**
   * Prepare the material for rendering.
   *
   * Determine whether blending is enabled for this material, and store the result.
   * @param[in] bufferIndex The current buffer index
   */
  void PrepareRender( BufferIndex bufferIndex );

  /**
   * Return the blend policy ( a combination of all the different shader hints, color, samper and image properties ).
   * This should only be called from the update thread
   * @return The material's blend policy
   */
  BlendPolicy GetBlendPolicy() const;

  /**
   * Set the blending options. This should only be called from the update thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions( BufferIndex updateBufferIndex, unsigned int options );

public: // Implementation of MaterialDataProvider

  /**
   * @copydoc MaterialDataProvider::GetBlendColor
   */
  virtual const Vector4& GetBlendColor(BufferIndex bufferIndex) const;

  /**
   * @copydoc MaterialDataProvider::GetBlendSrcFactorRgb
   */
  virtual BlendingFactor::Type GetBlendSrcFactorRgb(BufferIndex bufferIndex) const;

  /**
   * @copydoc MaterialDataProvider::GetBlendSrcFactorAlpha
   */
  virtual BlendingFactor::Type GetBlendSrcFactorAlpha( BufferIndex bufferIndex ) const;

  /**
   * @copydoc MaterialDataProvider::GetBlendDestFactorRgb
   */
  virtual BlendingFactor::Type GetBlendDestFactorRgb( BufferIndex bufferIndex ) const;

  /**
   * @copydoc MaterialDataProvider::GetBlendDestFactorAlpha
   */
  virtual BlendingFactor::Type GetBlendDestFactorAlpha( BufferIndex bufferIndex ) const;

  /**
   * @copydoc MaterialDataProvider::GetBlendEquationRgb
   */
  virtual BlendingEquation::Type GetBlendEquationRgb( BufferIndex bufferIndex ) const;

  /**
   * @copydoc MaterialDataProvider::GetBlendEquationAlpha
   */
  virtual BlendingEquation::Type GetBlendEquationAlpha( BufferIndex bufferIndex ) const;

public: // Implementation of ObjectOwnerContainer template methods
  /**
   * Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * Disconnect the object from the scene graph
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

public: // Implementation of ConnectionChangePropagator
  /**
   * @copydoc ConnectionChangePropagator::AddObserver
   */
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer);

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer);

public:
  /**
   * Get the shader effect of this material
   * @return the shader effect;
   */
  Shader* GetShader() const;

  /**
   * Get the samplers this material uses.
   * @return the samplers
   */
  Vector<Sampler*>& GetSamplers();

public: // UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  virtual void UniformMappingsChanged( const UniformMap& mappings );

public: // ConnectionChangePropagator::Observer

  /**
   * @copydoc ConnectionChangePropagator::ConnectionsChanged
   */
  virtual void ConnectionsChanged( PropertyOwner& owner );

  /**
   * @copydoc ConnectionChangePropagator::ConnectedUniformMapChanged
   */
  virtual void ConnectedUniformMapChanged( );

public: // PropertyOwner implementation
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

public: // Property data
  AnimatableProperty<Vector4> mColor;
  AnimatableProperty<Vector4> mBlendColor;
  DoubleBufferedProperty<int> mFaceCullingMode;
  DoubleBufferedProperty<int> mBlendingMode;
  DoubleBufferedProperty<int> mBlendingOptions;

private:
  Shader* mShader;
  Vector<Sampler*> mSamplers; // Not owned
  ConnectionChangePropagator mConnectionObservers;
  BlendPolicy mBlendPolicy; ///< The blend policy as determined by PrepareRender
};

inline void SetShaderMessage( EventThreadServices& eventThreadServices, const Material& material, const Shader& shader )
{
  typedef MessageValue1< Material, Shader* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetShader, const_cast<Shader*>(&shader) );
}

inline void AddSamplerMessage( EventThreadServices& eventThreadServices, const Material& material, const Sampler& sampler )
{
  typedef MessageValue1< Material, Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::AddSampler, const_cast<Sampler*>(&sampler) );
}

inline void RemoveSamplerMessage( EventThreadServices& eventThreadServices, const Material& material, const Sampler& sampler )
{
  typedef MessageValue1< Material, Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::RemoveSampler, const_cast<Sampler*>(&sampler) );
}

inline void SetBlendingOptionsMessage( EventThreadServices& eventThreadServices, const Material& material, unsigned int options )
{
  typedef MessageDoubleBuffered1< Material, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &material, &Material::SetBlendingOptions, options );
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H
