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

// CLASS HEADER
#include "scene-graph-renderer.h"

// INTERNAL HEADERS
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-material.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>


namespace // unnamed namespace
{

const unsigned int UNIFORM_MAP_READY      = 0;
const unsigned int COPY_UNIFORM_MAP       = 1;
const unsigned int REGENERATE_UNIFORM_MAP = 2;

//Memory pool used to allocate new renderers. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Renderer> gRendererMemoryPool;

void AddMappings( Dali::Internal::SceneGraph::CollectedUniformMap& localMap, const Dali::Internal::SceneGraph::UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  //   Any maps that aren't in localMap should be added in a single step
  Dali::Internal::SceneGraph::CollectedUniformMap newUniformMappings;

  for( unsigned int i=0, count=uniformMap.Count(); i<count; ++i )
  {
    Dali::Internal::SceneGraph::UniformPropertyMapping::Hash nameHash = uniformMap[i].uniformNameHash;
    bool found = false;

    for( Dali::Internal::SceneGraph::CollectedUniformMap::Iterator iter = localMap.Begin() ; iter != localMap.End() ; ++iter )
    {
      const Dali::Internal::SceneGraph::UniformPropertyMapping* map = (*iter);
      if( map->uniformNameHash == nameHash )
      {
        if( map->uniformName == uniformMap[i].uniformName )
        {
          found = true;
          break;
        }
      }
    }
    if( !found )
    {
      // it's a new mapping. Add raw ptr to temporary list
      newUniformMappings.PushBack( &uniformMap[i] );
    }
  }

  if( newUniformMappings.Count() > 0 )
  {
    localMap.Reserve( localMap.Count() + newUniformMappings.Count() );

    for( Dali::Internal::SceneGraph::CollectedUniformMap::Iterator iter = newUniformMappings.Begin(),
           end = newUniformMappings.End() ;
         iter != end ;
         ++iter )
    {
      const Dali::Internal::SceneGraph::UniformPropertyMapping* map = (*iter);
      localMap.PushBack( map );
    }
    //@todo MESH_REWORK Use memcpy to copy ptrs from one array to the other
  }
}
}

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Renderer* Renderer::New()
{
  return new ( gRendererMemoryPool.AllocateRawThreadSafe() ) Renderer();
}

Renderer::Renderer()
:mSceneController(0),
 mRenderer(NULL),
 mMaterial(NULL),
 mGeometry(NULL),
 mBlendColor(NULL),
 mBlendBitmask(0u),
 mFaceCullingMode( 0u ),
 mBlendingMode( Dali::BlendingMode::AUTO ),
 mReferenceCount(0),
 mRegenerateUniformMap(0),
 mResendDataProviders(false),
 mResendGeometry(false),
 mResourcesReady(false),
 mFinishedResourceAcquisition(false),
 mDepthIndex(0)
{
  mUniformMapChanged[0]=false;
  mUniformMapChanged[1]=false;

  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
}

Renderer::~Renderer()
{
  if (mMaterial)
  {
    mMaterial->RemoveConnectionObserver(*this);
    mMaterial=NULL;
  }
  if (mGeometry)
  {
    mGeometry->RemoveConnectionObserver(*this);
    mGeometry=NULL;
  }
}

void Renderer::operator delete( void* ptr )
{
  gRendererMemoryPool.FreeThreadSafe( static_cast<Renderer*>( ptr ) );
}


void Renderer::PrepareRender( BufferIndex updateBufferIndex )
{
  mResourcesReady = false;
  mFinishedResourceAcquisition = false;

  // Can only be considered ready when all the scene graph objects are connected to the renderer
  if( ( mGeometry ) && ( mGeometry->GetVertexBuffers().Count() > 0 ) &&
      ( mMaterial ) && ( mMaterial->GetShader() != NULL ) )
  {
    mMaterial->GetResourcesStatus( mResourcesReady, mFinishedResourceAcquisition );
  }

  if( mRegenerateUniformMap > UNIFORM_MAP_READY )
  {
    DALI_ASSERT_DEBUG( mGeometry != NULL && "No geometry available in DoPrepareRender()" );
    DALI_ASSERT_DEBUG( mMaterial != NULL && "No geometry available in DoPrepareRender()" );

    if( mRegenerateUniformMap == REGENERATE_UNIFORM_MAP)
    {
      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      localMap.Resize(0);

      const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();
      AddMappings( localMap, rendererUniformMap );

      AddMappings( localMap, mMaterial->GetUniformMap() );
      AddMappings( localMap, mMaterial->GetShader()->GetUniformMap() );
      AddMappings( localMap, mGeometry->GetUniformMap() );

    }
    else if( mRegenerateUniformMap == COPY_UNIFORM_MAP )
    {
      // Copy old map into current map
      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      CollectedUniformMap& oldMap = mCollectedUniformMap[ 1-updateBufferIndex ];

      localMap.Resize( oldMap.Count() );

      unsigned int index=0;
      for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter, ++index )
      {
        localMap[index] = *iter;
      }
    }

    mUniformMapChanged[updateBufferIndex] = true;
    mRegenerateUniformMap--;
  }

  if( mResendDataProviders )
  {
    RenderDataProvider* dataProvider = NewRenderDataProvider();

    typedef MessageValue1< Render::Renderer, OwnerPointer<RenderDataProvider> > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &Render::Renderer::SetRenderDataProvider, dataProvider );
    mResendDataProviders = false;
  }

  if( mResendGeometry )
  {
    // The first call to GetRenderGeometry() creates the geometry and sends it in a message
    RenderGeometry* geometry = mGeometry->GetRenderGeometry( mSceneController );

    typedef MessageValue1< Render::Renderer, RenderGeometry* > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    new (slot) DerivedType( mRenderer, &Render::Renderer::SetGeometry, geometry );
    mResendGeometry = false;
  }
}

void Renderer::SetMaterial( BufferIndex bufferIndex, Material* material)
{
  DALI_ASSERT_DEBUG( material != NULL && "Material pointer is NULL" );

  mMaterial = material;
  mMaterial->AddConnectionObserver( *this );
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  mResendDataProviders = true;
}

void Renderer::SetGeometry( BufferIndex bufferIndex, Geometry* geometry)
{
  DALI_ASSERT_DEBUG( geometry != NULL && "Geometry pointer is NULL");
  if( mGeometry)
  {
    mGeometry->RemoveConnectionObserver(*this);
    mGeometry->OnRendererDisconnect();
  }

  mGeometry = geometry;
  mGeometry->AddConnectionObserver( *this ); // Observe geometry connections / uniform mapping changes
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  if( mRenderer )
  {
    mResendGeometry = true;
  }
}

void Renderer::SetDepthIndex( int depthIndex )
{
  mDepthIndex = depthIndex;
}

void Renderer::SetFaceCullingMode( unsigned int faceCullingMode )
{
  if( mRenderer )
  {
    typedef MessageValue1< Render::Renderer, unsigned int > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( 0u, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &Render::Renderer::SetFaceCullingMode, faceCullingMode );
  }

  mFaceCullingMode = faceCullingMode;
}

void Renderer::SetBlendingMode( unsigned int blendingMode )
{
  mBlendingMode = static_cast< BlendingMode::Type >( blendingMode );
}

void Renderer::SetBlendingOptions( unsigned int options )
{
  if( mRenderer && mBlendBitmask != options)
  {
    typedef MessageValue1< Render::Renderer, unsigned int > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( 0u, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &Render::Renderer::SetBlendingBitMask, options );
  }

  mBlendBitmask = options;
}

void Renderer::SetBlendColor( const Vector4& blendColor )
{
  if( !mBlendColor )
  {
    mBlendColor = new Vector4( blendColor );
  }
  else
  {
    *mBlendColor = blendColor;
  }

  if( mRenderer )
  {
    typedef MessageValue1< Render::Renderer, const Vector4* > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( 0u, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &Render::Renderer::SetBlendColor, mBlendColor );
  }
}

void Renderer::EnablePreMultipliedAlpha( bool preMultipled )
{
  if( mRenderer )
  {
    typedef MessageValue1< Render::Renderer, bool > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( 0u, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &Render::Renderer::EnablePreMultipliedAlpha, preMultipled );
  }
  mPremultipledAlphaEnabled = preMultipled;
}

//Called when a node with this renderer is added to the stage
void Renderer::OnStageConnect()
{
  ++mReferenceCount;
  if( !mRenderer)
  {
    RenderDataProvider* dataProvider = NewRenderDataProvider();

    RenderGeometry* renderGeometry = mGeometry->GetRenderGeometry(mSceneController);
    mRenderer = Render::Renderer::New( dataProvider, renderGeometry,
                                       mBlendBitmask, mBlendColor,
                                       static_cast< Dali::Renderer::FaceCullingMode >( mFaceCullingMode ),
                                       mPremultipledAlphaEnabled );
    mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
    mResendDataProviders = false;
    mResendGeometry = false;
  }
}

//Called when the node with this renderer has gone out of the stage
void Renderer::OnStageDisconnect()
{
  --mReferenceCount;
  if( mReferenceCount == 0 )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
    if( mGeometry )
    {
      mGeometry->OnRendererDisconnect();
    }
    mRenderer = NULL;
  }
}

//Called when SceneGraph::Renderer is added to update manager ( that happens when an "event-thread renderer" is created )
void Renderer::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
  mSceneController = &sceneController;
}


//Called just before destroying the scene-graph renderer ( when the "event-thread renderer" is no longer referenced )
void Renderer::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  //Remove renderer from RenderManager
  if( mRenderer )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
    mRenderer = NULL;
    mSceneController = NULL;
  }
}

RenderDataProvider* Renderer::NewRenderDataProvider()
{
  RenderDataProvider* dataProvider = new RenderDataProvider();

  dataProvider->mUniformMapDataProvider = this;
  dataProvider->mShader = mMaterial->GetShader();

  size_t textureCount( mMaterial->GetTextureCount() );
  dataProvider->mTextures.resize( textureCount );
  for( unsigned int i(0); i<textureCount; ++i )
  {
    dataProvider->mTextures[i] = Render::Texture( mMaterial->GetTextureUniformName(i),
                                                  mMaterial->GetTextureId(i),
                                                  mMaterial->GetTextureSampler(i));
  }

  return dataProvider;
}

Render::Renderer& Renderer::GetRenderer()
{
  return *mRenderer;
}

const CollectedUniformMap& Renderer::GetUniformMap( BufferIndex bufferIndex ) const
{
  return mCollectedUniformMap[bufferIndex];
};

void Renderer::GetReadyAndComplete( bool& ready, bool& complete ) const
{
  ready = mResourcesReady;
  complete = mFinishedResourceAcquisition;
}

Renderer::Opacity Renderer::GetOpacity( BufferIndex updateBufferIndex, const Node& node ) const
{
  Renderer::Opacity opacity = Renderer::OPAQUE;

  if( mMaterial )
  {
    switch( mBlendingMode )
    {
      case BlendingMode::ON: // If the renderer should always be use blending
      {
        opacity = Renderer::TRANSLUCENT;
        break;
      }
      case BlendingMode::AUTO:
      {
        if(mMaterial->IsTranslucent() ) // If the renderer should determine opacity using the material
        {
          opacity = Renderer::TRANSLUCENT;
        }
        else // renderer should determine opacity using the actor color
        {
          float alpha = node.GetWorldColor( updateBufferIndex ).a;
          if( alpha <= FULLY_TRANSPARENT )
          {
            opacity = TRANSPARENT;
          }
          else if( alpha <= FULLY_OPAQUE )
          {
            opacity = TRANSLUCENT;
          }
        }
        break;
      }
      case BlendingMode::OFF: // the renderer should never use blending
      default:
      {
        opacity = Renderer::OPAQUE;
        break;
      }
    }
  }

  return opacity;
}

void Renderer::ConnectionsChanged( PropertyOwner& object )
{
  // One of our child objects has changed it's connections. Ensure the uniform
  // map gets regenerated during PrepareRender
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  // Ensure the child object pointers get re-sent to the renderer
  mResendDataProviders = true;
}

void Renderer::ConnectedUniformMapChanged()
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::UniformMappingsChanged( const UniformMap& mappings )
{
  // The mappings are either from PropertyOwner base class, or the Actor
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void Renderer::ObservedObjectDestroyed(PropertyOwner& owner)
{
  if( reinterpret_cast<PropertyOwner*>(mGeometry) == &owner )
  {
    mGeometry = NULL;
  }
  else if( reinterpret_cast<PropertyOwner*>(mMaterial) == &owner )
  {
    mMaterial = NULL;
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
