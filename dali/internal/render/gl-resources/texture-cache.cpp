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

#include <dali/internal/render/gl-resources/texture-cache.h>

#include <dali/integration-api/bitmap.h>

#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/common/post-process-resource-dispatcher.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-factory.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-observer.h>
#include <dali/internal/render/gl-resources/bitmap-texture.h>
#include <dali/internal/render/gl-resources/native-texture.h>
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>

#include <dali/integration-api/debug.h>

using Dali::Internal::Texture;
using Dali::Internal::FrameBufferTexture;
using Dali::Integration::Bitmap;

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gTextureCacheFilter = Debug::Filter::New(Debug::Concise, false, "LOG_TEXTURE_CACHE");
#endif
}


namespace Dali
{
namespace Internal
{

// value types used by messages
template <> struct ParameterType< Pixel::Format > : public BasicType< Pixel::Format > {};

namespace SceneGraph
{

TextureCache::TextureCache( RenderQueue& renderQueue,
                            PostProcessResourceDispatcher& postProcessResourceDispatcher,
                            Context& context)
: TextureCacheDispatcher(renderQueue),
  mPostProcessResourceDispatcher(postProcessResourceDispatcher),
  mContext(context),
  mDiscardBitmapsPolicy(ResourcePolicy::DISCARD),
  mRecycleConfiguration( DEFAULT_MAX_NUMBER_OF_RECYCLABLE_TEXTURES,
                         DEFAULT_WIDTH_OF_RECYCLABLE_TEXTURES,
                         DEFAULT_HEIGHT_OF_RECYCLABLE_TEXTURES,
                         DEFAULT_FORMAT_OF_RECYCLABLE_TEXTURES )
{
  mRecycledTextures.reserve( mRecycleConfiguration.maxNumberOfTextures );
}

TextureCache::~TextureCache()
{
}

void TextureCache::CreateTexture( ResourceId        id,
                                  unsigned int      width,
                                  unsigned int      height,
                                  Pixel::Format     pixelFormat,
                                  bool              clearPixels )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::CreateTexture(id=%i width:%u height:%u)\n", id, width, height);

  TexturePointer texture;

  if( IsRecycledTextureAvailable( width, height, pixelFormat ) )
  {
    texture = AssignRecycledTexture(Integration::BitmapPtr(), clearPixels);
  }
  else
  {
    texture = TextureFactory::NewBitmapTexture(width, height, pixelFormat, clearPixels, mContext, GetDiscardBitmapsPolicy() );
  }

  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddBitmap(ResourceId id, Integration::BitmapPtr bitmap)
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddBitmap(id=%i Bitmap:%p)\n", id, bitmap.Get());

  TexturePointer texture;

  if( IsRecycledTextureAvailable( bitmap ) )
  {
    texture = AssignRecycledTexture( bitmap, false );
  }
  else
  {
    texture = TextureFactory::NewBitmapTexture(bitmap.Get(), mContext, GetDiscardBitmapsPolicy());
  }

  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddNativeImage(ResourceId id, NativeImagePtr nativeImage)
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddNativeImage(id=%i NativeImg:%p)\n", id, nativeImage.Get());

  /// TODO - currently a new Texture is created even if we reuse the same NativeImage
  Texture* texture = TextureFactory::NewNativeImageTexture(*nativeImage, mContext);
  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddFrameBuffer(id=%i width:%u height:%u)\n", id, width, height);

  // Note: Do not throttle framebuffer generation - a request for a framebuffer should always be honoured
  // as soon as possible.
  Texture* texture = TextureFactory::NewFrameBufferTexture( width, height, pixelFormat, mContext );
  mFramebufferTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddFrameBuffer( ResourceId id, NativeImagePtr nativeImage )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddFrameBuffer(id=%i width:%u height:%u)\n", id, nativeImage->GetWidth(), nativeImage->GetHeight());

  // Note: Do not throttle framebuffer generation - a request for a framebuffer should always be honoured
  // as soon as possible.
  Texture* texture = TextureFactory::NewFrameBufferTexture( nativeImage, mContext );
  mFramebufferTextures.insert(TexturePair(id, texture));
}

void TextureCache::UpdateTexture( ResourceId id, Integration::BitmapPtr bitmap )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTexture(id=%i bitmap:%p )\n", id, bitmap.Get());

  TextureIter textureIter = mTextures.find(id);
  if( textureIter != mTextures.end() )
  {
    // we have reloaded the image from file, update texture
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->Update( bitmap.Get() );

      ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::UPLOADED );
      mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
    }
  }
}

void TextureCache::UpdateTextureArea( ResourceId id, const Dali::RectArea& area )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTextureArea(id=%i)\n", id );

  TextureIter textureIter = mTextures.find(id);
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->UpdateArea( area );

      ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::UPLOADED );
      mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
    }
  }
}

void TextureCache::AddBitmapUploadArray( ResourceId id, const BitmapUploadArray& uploadArray )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddBitmapUploadArray(id=%i )\n", id);

  TextureIter textureIter = mTextures.find(id);

  DALI_ASSERT_DEBUG( textureIter != mTextures.end() );
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      BitmapTexture* texture = static_cast< BitmapTexture* >( texturePtr.Get() );
      texture->UploadBitmapArray( uploadArray );
    }
  }
}

void TextureCache::ClearAreas( ResourceId id,
                               const BitmapClearArray& areaArray,
                               std::size_t blockSize,
                               uint32_t color )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::ClearAreas(id: %d)\n", id);
  TextureIter textureIter = mTextures.find(id);
  DALI_ASSERT_DEBUG( textureIter != mTextures.end() );
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      BitmapTexture* texture = static_cast< BitmapTexture* >( texturePtr.Get() );
      texture->ClearAreas( areaArray, blockSize, color );
    }
  }
}

void TextureCache::DiscardTexture( ResourceId id )
{
  bool deleted = false;
  bool recycled = false;

  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::DiscardTexture(id:%u)\n", id);

  if( mTextures.size() > 0)
  {
    TextureIter iter = mTextures.find(id);
    if( iter != mTextures.end() )
    {
      TexturePointer texturePtr = iter->second;
      if( texturePtr )
      {
        if( IsRecyclable(texturePtr) )
        {
          RecycleTexture(texturePtr);
        }
        else
        {
          // if valid texture pointer, cleanup GL resources
          texturePtr->GlCleanup();
        }
      }
      mTextures.erase(iter);
      deleted = true;
    }
  }

  if( mFramebufferTextures.size() > 0)
  {
    TextureIter iter = mFramebufferTextures.find(id);
    if( iter != mFramebufferTextures.end() )
    {
      TexturePointer texturePtr = iter->second;
      if( texturePtr )
      {
        // if valid texture pointer, cleanup GL resources
        texturePtr->GlCleanup();
      }
      mFramebufferTextures.erase(iter);
      deleted = true;
    }
  }

  if( deleted || recycled )
  {
    if( mObservers.size() > 0 )
    {
      TextureResourceObserversIter observersIter = mObservers.find(id);
      if( observersIter != mObservers.end() )
      {
        TextureObservers observers = observersIter->second;
        for( TextureObserversIter iter = observers.begin(); iter != observers.end(); ++iter )
        {
          TextureObserver* observer = *iter;
          observer->TextureDiscarded( id );
        }

        mObservers.erase( observersIter );
      }
    }

    // Tell resource manager
    ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::DELETED );
    mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
  }
}

void TextureCache::UpdateConfiguration( const TextureRecyclingConfiguration& recycleConfiguration)
{
  if( mRecycleConfiguration.width != recycleConfiguration.width ||
      mRecycleConfiguration.height != recycleConfiguration.height ||
      mRecycleConfiguration.pixelFormat != recycleConfiguration.pixelFormat )
  {
    TextureVector::iterator end = mRecycledTextures.end();
    TextureVector::iterator iter = mRecycledTextures.begin();
    for( ; iter != end; ++iter )
    {
      (*iter)->GlCleanup();    // Ensure recyclable GL textures are destroyed first
    }

    mRecycledTextures.clear(); // Destroy all texture objects in the recycle list
  }

  if( mRecycleConfiguration.maxNumberOfTextures < recycleConfiguration.maxNumberOfTextures )
  {
    // Increase number of recyclable textures
    mRecycledTextures.reserve(recycleConfiguration.maxNumberOfTextures);
  }
  else if( mRecycleConfiguration.maxNumberOfTextures > recycleConfiguration.maxNumberOfTextures )
  {
    // Delete excess recycled textures
    int excessTextures = mRecycledTextures.size() - recycleConfiguration.maxNumberOfTextures;
    if( excessTextures > 0 )
    {
      for( int i=0; i<excessTextures; ++i )
      {
        mRecycledTextures.back()->GlCleanup();
        mRecycledTextures.pop_back();
      }
    }
  }

  mRecycleConfiguration = recycleConfiguration;
}

void TextureCache::BindTexture( Texture *texture, ResourceId id, GLenum target, TextureUnit textureunit )
{
  bool uploaded = texture->Bind(target, textureunit);
  if( uploaded )
  {
    ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::UPLOADED );
    mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
  }
}

Texture* TextureCache::GetTexture(ResourceId id)
{
  Texture* texture = NULL;
  TextureIter iter = mTextures.find(id);

  if( iter != mTextures.end() )
  {
    TexturePointer texturePtr = iter->second;
    if( texturePtr )
    {
      texture = texturePtr.Get();
    }
  }

  if( texture == NULL )
  {
    TextureIter iter = mFramebufferTextures.find(id);
    if( iter != mFramebufferTextures.end() )
    {
      TexturePointer texturePtr = iter->second;
      if( texturePtr )
      {
        texture = texturePtr.Get();
      }
    }
  }

  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::GetTexture(id:%u) : %p\n", id, texture);

  return texture;
}

FrameBufferTexture* TextureCache::GetFramebuffer(ResourceId id)
{
  FrameBufferTexture* offscreen = NULL;
  TextureIter iter = mFramebufferTextures.find(id);

  DALI_ASSERT_DEBUG( iter != mFramebufferTextures.end() );

  if( iter != mFramebufferTextures.end() )
  {
    TexturePointer texturePtr = iter->second;
    if( texturePtr )
    {
      offscreen = dynamic_cast< FrameBufferTexture* >( texturePtr.Get() );
    }
  }
  DALI_ASSERT_DEBUG( offscreen );

  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::GetFramebuffer(id:%u) : %p\n", id, offscreen);

  return offscreen;
}

void TextureCache::AddObserver( ResourceId id, TextureObserver* observer )
{
  TextureResourceObserversIter observersIter = mObservers.find(id);
  if( observersIter != mObservers.end() )
  {
    TextureObservers& observers = observersIter->second;
    bool foundObserver = false;
    for( TextureObserversIter iter = observers.begin(); iter != observers.end(); ++iter )
    {
      if( *iter == observer )
      {
        foundObserver = true;
        break;
      }
    }
    if( ! foundObserver )
    {
      observers.push_back(observer);
    }
  }
  else
  {
    TextureObservers observers;
    observers.push_back(observer);
    mObservers.insert(std::pair<ResourceId, TextureObservers>(id, observers));
  }
}

void TextureCache::RemoveObserver( ResourceId id, TextureObserver* observer )
{
  TextureResourceObserversIter observersIter = mObservers.find(id);
  if( observersIter != mObservers.end() )
  {
    TextureObservers& observers = observersIter->second;
    for( TextureObserversIter iter = observers.begin(); iter != observers.end(); ++iter )
    {
      if(*iter == observer)
      {
        observers.erase(iter);
        break;
      }
    }
  }
}

void TextureCache::GlContextDestroyed()
{
  TextureIter end = mTextures.end();
  TextureIter iter = mTextures.begin();
  for( ; iter != end; ++iter )
  {
    (*iter->second).GlContextDestroyed(); // map holds intrusive pointers
  }

  end = mFramebufferTextures.end();
  iter = mFramebufferTextures.begin();
  for( ; iter != end; ++iter )
  {
    (*iter->second).GlContextDestroyed(); // map holds intrusive pointers
  }

  mRecycledTextures.clear();
}

void TextureCache::SetDiscardBitmapsPolicy( ResourcePolicy::Discardable policy )
{
  DALI_LOG_INFO( gTextureCacheFilter, Debug::General, "TextureCache::SetDiscardBitmapsPolicy(%s)\n",
                 policy==ResourcePolicy::RETAIN?"RETAIN":"DISCARD" );
  mDiscardBitmapsPolicy = policy;
}

ResourcePolicy::Discardable TextureCache::GetDiscardBitmapsPolicy()
{
  return mDiscardBitmapsPolicy;
}

bool TextureCache::IsRecycledTextureAvailable( Integration::BitmapPtr bitmap )
{
  bool textureAvailable = false;

  if( mRecycledTextures.size() > 0 )
  {
    Integration::Bitmap::PackedPixelsProfile* profile = bitmap->GetPackedPixelsProfile();

    if( profile &&
        profile->GetBufferWidth() == mRecycleConfiguration.width &&
        profile->GetBufferHeight() == mRecycleConfiguration.height &&
        bitmap->GetPixelFormat() == mRecycleConfiguration.pixelFormat )
    {
      textureAvailable = true;
    }
  }
  return textureAvailable;
}

bool TextureCache::IsRecycledTextureAvailable( unsigned int width, unsigned int height, Pixel::Format pixelFormat )
{
  bool textureAvailable = false;

  if( mRecycledTextures.size() > 0 )
  {
    if( width == mRecycleConfiguration.width &&
        height == mRecycleConfiguration.height &&
        pixelFormat == mRecycleConfiguration.pixelFormat )
    {
      textureAvailable = true;
    }
  }
  return textureAvailable;
}

bool TextureCache::IsRecyclable( TexturePointer texture )
{
  bool recyclable = false;

  BitmapTexture* bitmapTexture = dynamic_cast<BitmapTexture*>(texture.Get());

  if( mRecycledTextures.size() < mRecycleConfiguration.maxNumberOfTextures &&
      bitmapTexture &&
      bitmapTexture->GetWidth() == mRecycleConfiguration.width &&
      bitmapTexture->GetHeight() == mRecycleConfiguration.height &&
      bitmapTexture->GetPixelFormat() == mRecycleConfiguration.pixelFormat )
  {
    recyclable = true;
  }
  return recyclable;
}

TexturePointer TextureCache::AssignRecycledTexture( Integration::BitmapPtr bitmap, bool clearPixels )
{
  TexturePointer last = mRecycledTextures.back();
  BitmapTexture* bitmapTexture = dynamic_cast<BitmapTexture*>(last.Get());
  if( bitmapTexture != NULL )
  {
    // Reset sampler, Reset image width/height to texture width/height
    bitmapTexture->Reset(bitmap.Get(), clearPixels);
  }
  mRecycledTextures.pop_back();

  return last;
}

void TextureCache::RecycleTexture( TexturePointer texture )
{
  mRecycledTextures.push_back( texture );
}

/********************************************************************************
 **********************  Implements TextureCacheDispatcher  *********************
 ********************************************************************************/

void TextureCache::DispatchCreateTexture( ResourceId        id,
                                          unsigned int      width,
                                          unsigned int      height,
                                          Pixel::Format     pixelFormat,
                                          bool              clearPixels )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue5< TextureCache, ResourceId, unsigned int, unsigned int, Pixel::Format, bool > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::CreateTexture, id, width, height, pixelFormat, clearPixels );
  }
}

void TextureCache::DispatchCreateTextureForBitmap( ResourceId id, Bitmap* bitmap )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Integration::BitmapPtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddBitmap, id, bitmap );
  }
}

void TextureCache::DispatchCreateTextureForNativeImage( ResourceId id, NativeImagePtr nativeImage )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, NativeImagePtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddNativeImage, id, nativeImage );
  }
}

void TextureCache::DispatchCreateTextureForFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue4< TextureCache, ResourceId, unsigned int, unsigned int, Pixel::Format > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddFrameBuffer, id, width, height, pixelFormat );
  }
}

void TextureCache::DispatchCreateTextureForFrameBuffer( ResourceId id, NativeImagePtr nativeImage )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, NativeImagePtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddFrameBuffer, id, nativeImage );
  }
}

void TextureCache::DispatchUpdateTexture( ResourceId id, Bitmap* bitmap )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Integration::BitmapPtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::UpdateTexture, id, bitmap );
  }
}

void TextureCache::DispatchUpdateTextureArea( ResourceId id, const Dali::RectArea& area )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Dali::RectArea > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::UpdateTextureArea, id, area );
  }
}

void TextureCache::DispatchUploadBitmapArrayToTexture( ResourceId id, const BitmapUploadArray& uploadArray )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, BitmapUploadArray > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddBitmapUploadArray, id, uploadArray );
  }
}

void TextureCache::DispatchClearAreas( ResourceId id, const BitmapClearArray& areaArray, std::size_t blockSize, uint32_t color )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue4< TextureCache, ResourceId, BitmapClearArray, std::size_t, uint32_t > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::ClearAreas, id, areaArray, blockSize, color );
  }
}

void TextureCache::DispatchDiscardTexture( ResourceId id )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue1< TextureCache, ResourceId > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::DiscardTexture, id );
  }
}

void TextureCache::DispatchUpdateConfiguration( TextureRecyclingConfiguration config )
{
  typedef MessageValue1< TextureCache, TextureRecyclingConfiguration > DerivedType;
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof(DerivedType));
  new (slot) DerivedType( this, &TextureCache::UpdateConfiguration, config );
}

} // SceneGraph

} // Internal

} // Dali
