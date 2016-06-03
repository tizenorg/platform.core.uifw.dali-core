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
 *
 */

// CLASS HEADER
#include <dali/internal/render/gl-resources/texture-cache.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/resources/resource-client.h> // For RectArea
#include <dali/internal/render/common/texture-uploaded-dispatcher.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-factory.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/gl-resources/texture-observer.h>
#include <dali/internal/render/gl-resources/bitmap-texture.h>
#include <dali/internal/render/gl-resources/native-texture.h>
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/gl-resources/gl-texture.h>

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


namespace SceneGraph
{

namespace
{

/**
 * @brief Forward to all textures in container the news that the GL Context is down.
 */
void GlContextDestroyed( TextureContainer& textures )
{
  TextureIter end = textures.end();
  TextureIter iter = textures.begin();
  for( ; iter != end; ++iter )
  {
    (*iter->second).GlContextDestroyed();
  }
}

}

TextureCache::TextureCache( RenderQueue& renderQueue,
                            TextureUploadedDispatcher& postProcessResourceDispatcher,
                            Context& context)
: mTextureUploadedDispatcher(postProcessResourceDispatcher),
  mContext(context),
  mDiscardBitmapsPolicy(ResourcePolicy::OWNED_DISCARD)
{
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

  Texture* texture = TextureFactory::NewBitmapTexture(width, height, pixelFormat, clearPixels, mContext, GetDiscardBitmapsPolicy() );
  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddBitmap(ResourceId id, Integration::BitmapPtr bitmap)
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddBitmap(id=%i Bitmap:%p)\n", id, bitmap.Get());

  Texture* texture = TextureFactory::NewBitmapTexture(bitmap.Get(), mContext, GetDiscardBitmapsPolicy());
  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddNativeImage(ResourceId id, NativeImageInterfacePtr nativeImage)
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddNativeImage(id=%i NativeImg:%p)\n", id, nativeImage.Get());

  /// WARNING - currently a new Texture is created even if we reuse the same NativeImage
  Texture* texture = TextureFactory::NewNativeImageTexture(*nativeImage, mContext);
  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddFrameBuffer(id=%i width:%u height:%u)\n", id, width, height);

  // Note: Do not throttle framebuffer generation - a request for a framebuffer should always be honoured
  // as soon as possible.
  Texture* texture = TextureFactory::NewFrameBufferTexture( width, height, pixelFormat, bufferFormat, mContext );
  mFramebufferTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddFrameBuffer( ResourceId id, NativeImageInterfacePtr nativeImage )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddFrameBuffer(id=%i width:%u height:%u)\n", id, nativeImage->GetWidth(), nativeImage->GetHeight());

  // Note: Do not throttle framebuffer generation - a request for a framebuffer should always be honoured
  // as soon as possible.
  Texture* texture = TextureFactory::NewFrameBufferTexture( nativeImage, mContext );
  mFramebufferTextures.insert(TexturePair(id, texture));
}

void TextureCache::CreateGlTexture( ResourceId id )
{
  TextureIter textureIter = mTextures.find(id);
  // If we found a non-null texture object pointer for the resource id, force it
  // to eagerly allocate a backing GL texture:
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->CreateGlTexture();
    }
  }
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

      ResourceId ppRequest( id );
      mTextureUploadedDispatcher.DispatchTextureUploaded(ppRequest);
    }
  }
}

void TextureCache::UpdateTexture( ResourceId id, Integration::BitmapPtr bitmap, std::size_t xOffset, std::size_t yOffset )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTexture(id=%i bitmap:%p )\n", id, bitmap.Get());

  TextureIter textureIter = mTextures.find(id);
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->Update( bitmap.Get(), xOffset, yOffset );

      ResourceId ppRequest( id  );
      mTextureUploadedDispatcher.DispatchTextureUploaded(ppRequest);
    }
  }
}

void TextureCache::UpdateTexture( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTexture(destId=%i srcId=%i )\n", destId, srcId );

  BitmapTexture* srcTexture = TextureCache::GetBitmapTexture( srcId );
  Integration::BitmapPtr srcBitmap = ( srcTexture != NULL ) ? srcTexture->GetBitmap() : NULL;

  if( srcBitmap )
  {
    UpdateTexture( destId, srcBitmap, xOffset, yOffset );
  }
}

void TextureCache::UpdateTexture( ResourceId id, PixelDataPtr pixelData, std::size_t xOffset, std::size_t yOffset )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTexture(id=%i pixel data:%p )\n", id, pixelData.Get());

  TextureIter textureIter = mTextures.find(id);
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->Update( pixelData.Get(), xOffset, yOffset );

      ResourceId ppRequest( id );
      mTextureUploadedDispatcher.DispatchTextureUploaded(ppRequest);
    }
  }
}

void TextureCache::UpdateTextureArea( ResourceId id, const RectArea& area )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTextureArea(id=%i)\n", id );

  TextureIter textureIter = mTextures.find(id);
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->UpdateArea( area );

      ResourceId ppRequest( id );
      mTextureUploadedDispatcher.DispatchTextureUploaded(ppRequest);
    }
  }
}

void TextureCache::DiscardTexture( ResourceId id )
{
  bool deleted = false;

  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::DiscardTexture(id:%u)\n", id);

  if( mTextures.size() > 0)
  {
    TextureIter iter = mTextures.find(id);
    if( iter != mTextures.end() )
    {
      TexturePointer texturePtr = iter->second;
      if( texturePtr )
      {
        // if valid texture pointer, cleanup GL resources
        texturePtr->GlCleanup();
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

  if(deleted)
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
  }
}

bool TextureCache::BindTexture( Texture *texture, ResourceId id, GLenum target, TextureUnit textureunit )
{
  unsigned int glTextureId = texture->GetTextureId();

  bool success = texture->Bind(target, textureunit);
  bool created = ( glTextureId == 0 ) && ( texture->GetTextureId() != 0 );

  if( created && texture->UpdateOnCreate() ) // i.e. the pixel data was sent to GL
  {
    ResourceId ppRequest( id );
    mTextureUploadedDispatcher.DispatchTextureUploaded(ppRequest);
  }
  return success;
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

BitmapTexture* TextureCache::GetBitmapTexture(ResourceId id)
{
  BitmapTexture* texture = NULL;
  TextureIter iter = mTextures.find( id );

  if( iter != mTextures.end() )
  {
    TexturePointer texturePtr = iter->second;
    if( texturePtr )
    {
      texture = dynamic_cast< BitmapTexture* >( texturePtr.Get() );
    }
  }

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
  SceneGraph::GlContextDestroyed( mTextures );
  SceneGraph::GlContextDestroyed( mFramebufferTextures );
}

void TextureCache::SetDiscardBitmapsPolicy( ResourcePolicy::Discardable policy )
{
  DALI_LOG_INFO( gTextureCacheFilter, Debug::General, "TextureCache::SetDiscardBitmapsPolicy(%s)\n",
                 policy==ResourcePolicy::OWNED_DISCARD?"DISCARD":"RETAIN" );
  mDiscardBitmapsPolicy = policy;
}

ResourcePolicy::Discardable TextureCache::GetDiscardBitmapsPolicy()
{
  return mDiscardBitmapsPolicy;
}

} // SceneGraph

} // Internal

} // Dali
