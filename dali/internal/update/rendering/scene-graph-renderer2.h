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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-boolean.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/render/data-providers/geometry-data-provider.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/renderers/render-renderer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

class R3nderer;
typedef std::vector< R3nderer* > RendererContainer;
typedef RendererContainer::iterator RendererIter;
typedef RendererContainer::const_iterator RendererConstIter;

class Material;
class Geometry;
class NewRenderer;


class R3nderer :  public PropertyOwner,             //Has properties
                  public UniformMapDataProvider,    //Allows renders in Render thread to get uniform mappings
                  public UniformMap::Observer,      //@TODO: FERRAN Investigate why this is needed...
                  public ConnectionChangePropagator::Observer //@TODO: FERRAN Investigate why this is needed...
{
public:

  R3nderer();

  /**
   * Destructor
   */
  virtual ~R3nderer();

  /**
   * Set the material for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] material The material this renderer will use
   */
  void SetMaterial( BufferIndex bufferIndex, Material* material);
  void SetGeometry( BufferIndex bufferIndex, Geometry* material);
  void SetDepthIndex( int depthIndex );

  Material& GetMaterial()
  {
    return *mMaterial;
  }

  Geometry& GetGeometry()
  {
    return *mGeometry;
  }


  void OnStageConnect(Node* node);
  void OnStageDisconnect(const Node* node);

  void PrepareRender( BufferIndex updateBufferIndex, Node* node );
  void AddMappings( CollectedUniformMap& localMap, const UniformMap& uniformMap );

  //NewRenderer* GetRenderer();
  Renderer& GetRenderer();
  RenderDataProvider* NewRenderDataProvider();

  virtual int GetDepthIndex()
  {
    return mDepthIndex;
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
  virtual void UniformMappingsChanged( const UniformMap& mappings ){};

public: // ConnectionChangePropagator::Observer

  /**
   * @copydoc ConnectionChangePropagator::ConnectionsChanged
   */
  virtual void ConnectionsChanged( PropertyOwner& owner ){};

  /**
   * @copydoc ConnectionChangePropagator::ConnectedUniformMapChanged
   */
  virtual void ConnectedUniformMapChanged( ){};

public: // PropertyOwner implementation
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex ){};

public: //Uniform map provider

  virtual bool GetUniformMapChanged( BufferIndex bufferIndex ) const{ return mUniformMapChanged[bufferIndex];}
  virtual const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const;

private:
  SceneController* mSceneController;

  NewRenderer* mRenderer; ///< Raw pointer to the new renderer (that's owned by RenderManager)
  Material* mMaterial; ///< The material this renderer uses. (Not owned)
  Geometry* mGeometry; ///< The geometry this renderer uses. (Not owned)

  CollectedUniformMap mCollectedUniformMap[2];
  int mRegenerateUniformMap;      ///< 2 if the map should be regenerated, 1 if it should be copied.
  bool mUniformMapChanged[2];     ///< Records if the uniform map has been altered this frame
  bool mResendDataProviders : 1;  ///< True if the data providers should be resent to the renderer
  bool mResendGeometry      : 1;  ///< True if geometry should be resent to the renderer
  bool mUseBlend            : 1;  ///< True if the attachment & renderer should be considered opaque for sorting and blending.

  unsigned int mReferenceCount;
public:
  int mDepthIndex; ///< Used only in PrepareRenderInstructions
};

// Messages for RendererAttachment

inline void SetMaterialMessage( EventThreadServices& eventThreadServices, const R3nderer& renderer, const Material& material )
{
  typedef MessageDoubleBuffered1< R3nderer, Material* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &R3nderer::SetMaterial, const_cast<Material*>(&material) );
}

inline void SetGeometryMessage( EventThreadServices& eventThreadServices, const R3nderer& renderer, const Geometry& geometry )
{
  typedef MessageDoubleBuffered1< R3nderer, Geometry* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &R3nderer::SetGeometry, const_cast<Geometry*>(&geometry) );
}

inline void SetDepthIndexMessage( EventThreadServices& eventThreadServices, const R3nderer& attachment, int depthIndex )
{
  typedef MessageValue1< R3nderer, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &R3nderer::SetDepthIndex, depthIndex );
}

inline void OnStageConnectMessage( EventThreadServices& eventThreadServices, const R3nderer& renderer, const Node* node )
{
  typedef MessageValue1< R3nderer, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &R3nderer::OnStageConnect, const_cast<Node*>(node) );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
