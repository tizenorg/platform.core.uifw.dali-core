#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER2_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER2_H

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


#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/renderer.h> // Dali::Renderer
#include <dali/internal/common/blending-options.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
//todor
#include <string>

namespace Dali
{
namespace Internal
{

namespace Render
{
class Renderer;
}

namespace SceneGraph
{
class SceneController;

class Renderer;
typedef Dali::Vector< Renderer* > RendererContainer;
typedef RendererContainer::Iterator RendererIter;
typedef RendererContainer::ConstIterator RendererConstIter;

class Material;
class Geometry;

class Renderer :  public PropertyOwner,
                  public UniformMapDataProvider,
                  public UniformMap::Observer,
                  public ConnectionChangePropagator::Observer
{
public:

  enum Opacity
  {
    OPAQUE,
    TRANSPARENT,
    TRANSLUCENT
  };

  /**
   * Construct a new Renderer
   */
  static Renderer* New();

  /**
   * Destructor
   */
  virtual ~Renderer();

  //todor
  void SetName( std::string name );
  std::string GetName();
  void SetClippingMode( Dali::Renderer::ClippingMode mode );
  Dali::Renderer::ClippingMode GetClippingMode();

  /**
   * Overriden delete operator
   * Deletes the renderer from its global memory pool
   */
  void operator delete( void* ptr );

  /**
   * Set the material for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] material The material this renderer will use
   */
  void SetMaterial( BufferIndex bufferIndex, Material* material);

  /**
   * Get the material of this renderer
   * @return the material this renderer uses
   */
  Material& GetMaterial()
  {
    return *mMaterial;
  }

  /**
   * Set the geometry for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] geometry The geometry this renderer will use
   */
  void SetGeometry( BufferIndex bufferIndex, Geometry* material);

  /**
   * Get the geometry of this renderer
   * @return the geometry this renderer uses
   */
  Geometry& GetGeometry()
  {
    return *mGeometry;
  }

  /**
   * Set the depth index
   * @param[in] depthIndex the new depth index to use
   */
  void SetDepthIndex( int depthIndex );

  /**
   * @brief Get the depth index
   * @return The depth index
   */
  int GetDepthIndex() const
  {
    return mDepthIndex;
  }

  /**
   * Set the face culling mode
   * @param[in] faceCullingMode to use
   */
  void SetFaceCullingMode( unsigned int faceCullingMode );

  /**
   * Set the blending mode
   * @param[in] blendingMode to use
   */
  void SetBlendingMode( unsigned int blendingMode );

  /**
   * Set the blending options. This should only be called from the update thread.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions( unsigned int options );

  /**
   * Set the blend color for blending operation
   * @param blendColor to pass to GL
   */
  void SetBlendColor( const Vector4& blendColor );

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   *
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha( bool preMultipled );

  /**
   * Called when an actor with this renderer is added to the stage
   */
  void OnStageConnect();

  /*
   * Called when an actor with this renderer is removed from the stage
   */
  void OnStageDisconnect();

  /**
   * Prepare the object for rendering.
   * This is called by the UpdateManager when an object is due to be rendered in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PrepareRender( BufferIndex updateBufferIndex );

  /*
   * Retrieve the Render thread renderer
   * @return The associated render thread renderer
   */
  Render::Renderer& GetRenderer();

  /**
   * Check whether the renderer has been marked as ready to render
   * ready means that renderer has all resources and should produce correct result
   * complete means all resources have finished loading
   * It's possible that renderer is complete but not ready,
   * for example in case of resource load failed
   * @param[out] ready TRUE if the renderer has resources to render
   * @param[out] complete TRUE if the renderer resources are complete
   */
  void GetReadyAndComplete( bool& ready, bool& complete ) const;

  /**
   * Query whether the renderer is fully opaque, fully transparent or transparent.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return OPAQUE if fully opaque, TRANSPARENT if fully transparent and TRANSLUCENT if in between
   */
  Opacity GetOpacity( BufferIndex updateBufferIndex, const Node& node ) const;

  /**
   * Query whether the renderer is currently in use by an actor on the stage
   */
  bool IsReferenced() const
  {
    return mReferenceCount > 0;
  }


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
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer){};

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer){};

public:


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

  /**
   * @copydoc ConnectionChangePropagator::ConnectedUniformMapChanged
   */
  virtual void ObservedObjectDestroyed(PropertyOwner& owner);

public: // PropertyOwner implementation
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex ){};

public: // From UniformMapDataProvider

  /**
   * @copydoc UniformMapDataProvider::GetUniformMapChanged
   */
  virtual bool GetUniformMapChanged( BufferIndex bufferIndex ) const{ return mUniformMapChanged[bufferIndex];}

  /**
   * @copydoc UniformMapDataProvider::GetUniformMap
   */
  virtual const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const;

private:

  /**
   * Protected constructor; See also Renderer::New()
   */
  Renderer();

  /**
   * Helper function to create a new render data provider
   * @return the new (initialized) data provider
   */
  RenderDataProvider* NewRenderDataProvider();

private:

  SceneController* mSceneController;  ///< Used for initializing renderers whilst attached
  Render::Renderer*  mRenderer;    ///< Raw pointer to the new renderer (that's owned by RenderManager)
  Material*          mMaterial;    ///< The material this renderer uses. (Not owned)
  Geometry*          mGeometry;    ///< The geometry this renderer uses. (Not owned)

  Vector4*                        mBlendColor;      ///< The blend color for blending operation
  unsigned int                    mBlendBitmask;    ///< The bitmask of blending options
  Dali::Renderer::FaceCullingMode mFaceCullingMode; ///< The mode of face culling
  BlendingMode::Type              mBlendingMode;    ///< The mode of blending

  CollectedUniformMap mCollectedUniformMap[2]; ///< Uniform maps collected by the renderer
  unsigned int mReferenceCount;                ///< Number of nodes currently using this renderer
  unsigned int mRegenerateUniformMap;          ///< 2 if the map should be regenerated, 1 if it should be copied.
  unsigned char mResendFlag;                    ///< Indicate whether data should be resent to the renderer
  bool         mUniformMapChanged[2];          ///< Records if the uniform map has been altered this frame
  bool         mResourcesReady;                ///< Set during the Update algorithm; true if the attachment has resources ready for the current frame.
  bool         mFinishedResourceAcquisition;   ///< Set during DoPrepareResources; true if ready & all resource acquisition has finished (successfully or otherwise)
  bool         mPremultipledAlphaEnabled;      ///< Flag indicating whether the Pre-multiplied Alpha Blending is required

  std::string mName;//todor
  Dali::Renderer::ClippingMode mClippingMode;//todor

public:
  int mDepthIndex; ///< Used only in PrepareRenderInstructions
};


/// Messages
inline void SetMaterialMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const Material& material )
{
  typedef MessageDoubleBuffered1< Renderer, Material* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetMaterial, const_cast<Material*>(&material) );
}

inline void SetGeometryMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const Geometry& geometry )
{
  typedef MessageDoubleBuffered1< Renderer, Geometry* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetGeometry, const_cast<Geometry*>(&geometry) );
}

inline void SetDepthIndexMessage( EventThreadServices& eventThreadServices, const Renderer& attachment, int depthIndex )
{
  typedef MessageValue1< Renderer, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &Renderer::SetDepthIndex, depthIndex );
}

inline void SetFaceCullingModeMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, Dali::Renderer::FaceCullingMode faceCullingMode )
{
  typedef MessageValue1< Renderer, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetFaceCullingMode, faceCullingMode );
}

inline void SetBlendingModeMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, BlendingMode::Type blendingMode )
{
  typedef MessageValue1< Renderer, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetBlendingMode, blendingMode );
}

inline void SetBlendingOptionsMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, unsigned int options )
{
  typedef MessageValue1< Renderer, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetBlendingOptions, options );
}

inline void SetBlendColorMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const Vector4& blendColor )
{
  typedef MessageValue1< Renderer, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::SetBlendColor, blendColor );
}

inline void SetEnablePreMultipliedAlphaMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, bool preMultiplied )
{
  typedef MessageValue1< Renderer, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &renderer, &Renderer::EnablePreMultipliedAlpha, preMultiplied );
}

inline void OnStageConnectMessage( EventThreadServices& eventThreadServices, const Renderer& renderer )
{
  typedef Message< Renderer > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::OnStageConnect );
}

inline void OnStageDisconnectMessage( EventThreadServices& eventThreadServices, const Renderer& renderer )
{
  typedef Message< Renderer > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::OnStageDisconnect );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
