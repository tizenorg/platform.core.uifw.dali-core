/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "scene-graph-texture-set.h"

// INTERNAL HEADERS
#include <dali/public-api/actors/blending.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/devel-api/rendering/texture-set.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/update/resources/texture-metadata.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/common/memory-pool-object-allocator.h>

namespace //Unnamed namespace
{
//Memory pool used to allocate new texture sets. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::TextureSet> gTextureSetMemoryPool;
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

TextureSet* TextureSet::New()
{
  return new ( gTextureSetMemoryPool.AllocateRawThreadSafe() ) TextureSet();
}

TextureSet::TextureSet()
: mSamplers(),
  mTextureId(),
  mConnectionObservers(),
  mResourcesReady( false ),
  mFinishedResourceAcquisition( false ),
  mChanged( true ),
  mHasAlpha( false )
{
  // Observe own property-owner's uniform map
  AddUniformMapObserver( *this );
}

TextureSet::~TextureSet()
{
  mConnectionObservers.Destroy( *this );
}

void TextureSet::operator delete( void* ptr )
{
  gTextureSetMemoryPool.FreeThreadSafe( static_cast<TextureSet*>( ptr ) );
}

void TextureSet::Prepare( const ResourceManager& resourceManager )
{
  if( mChanged )
  {
    unsigned int opaqueCount = 0;
    unsigned int completeCount = 0;
    unsigned int failedCount = 0;
    unsigned int frameBufferCount = 0;
    const std::size_t textureCount( mTextureId.Count() );
    if( textureCount > 0 )
    {
      for( unsigned int i(0); i<textureCount; ++i )
      {
        const ResourceId textureId = mTextureId[ i ];
        TextureMetadata* metadata = NULL;
        if( textureId != Integration::InvalidResourceId )
        {
          // if there is metadata, resource is loaded
          if( resourceManager.GetTextureMetadata( textureId, metadata ) )
          {
            DALI_ASSERT_DEBUG( metadata );
            // metadata is valid pointer from now on
            if( metadata->IsFullyOpaque() )
            {
              ++opaqueCount;
            }

            if( metadata->IsFramebuffer() )
            {
              if( metadata->HasFrameBufferBeenRenderedTo() )
              {
                ++completeCount;
              }
              else
              {
                frameBufferCount++;
              }
            }
            else
            {
              // loaded so will complete this frame
              ++completeCount;
            }
            // no point checking failure as there is no metadata for failed loads
          }
          // if no metadata, loading can be failed
          else if( resourceManager.HasResourceLoadFailed( textureId ) )
          {
            ++failedCount;
          }
        }
        else
        {
          //If the texture is not valid it is considerer opaque and complete
          ++opaqueCount;
          ++completeCount;
        }
      }
    }

    mHasAlpha = ( opaqueCount != textureCount );

    // ready for rendering when all textures are either successfully loaded or they are FBOs
    mResourcesReady = (completeCount + frameBufferCount >= textureCount);

    // Texture set is complete if all resources are either loaded or failed or, if they are FBOs have been rendererd to
    mFinishedResourceAcquisition = ( completeCount + failedCount == textureCount );

    if( mFinishedResourceAcquisition )
    {
      // Texture set is now considered not changed
      mChanged = false;
    }
  }
}

void TextureSet::SetImage( size_t index,  ResourceId imageId )
{
  if( mTextureId.Size() < index + 1 )
  {
    mTextureId.Resize( index + 1 );
    mSamplers.Resize( index + 1 );
    mSamplers[index] = NULL;
  }

  mTextureId[index] = imageId;

  mChanged = true;
  mConnectionObservers.ConnectionsChanged(*this);
}

void TextureSet::SetSampler( size_t index, Render::Sampler* sampler )
{
  if( mSamplers.Size() < index + 1 )
  {
    mSamplers.Resize( index + 1 );
    mTextureId.Resize( index + 1 );
    mTextureId[index] = Integration::InvalidResourceId;
  }

  mSamplers[index] = sampler;

  mChanged = true;
  mConnectionObservers.ConnectionsChanged(*this);
}

bool TextureSet::HasAlpha() const
{
  return mHasAlpha;
}

void TextureSet::GetResourcesStatus( bool& resourcesReady, bool& finishedResourceAcquisition )
{
  resourcesReady = mResourcesReady;
  finishedResourceAcquisition = mFinishedResourceAcquisition;
}

void TextureSet::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void TextureSet::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void TextureSet::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void TextureSet::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void TextureSet::UniformMappingsChanged( const UniformMap& mappings )
{
  // Our uniform map  has changed.
  // Inform connected observers.
  mConnectionObservers.ConnectedUniformMapChanged();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
