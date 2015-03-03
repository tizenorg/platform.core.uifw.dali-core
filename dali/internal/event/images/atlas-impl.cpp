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

// CLASS HEADER
#include <dali/internal/event/images/atlas-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/buffer-image-impl.h>
#include <dali/internal/event/images/image-factory.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::Atlas ), typeid( Dali::Image ), NULL );
}

Atlas* Atlas::New( std::size_t width,
                   std::size_t height,
                   Pixel::Format pixelFormat )
{
  return new Atlas( width, height, pixelFormat );
}

void Atlas::Clear( const Vector4& color  )
{
  AllocateAtlas();
  ResourceId destId = GetResourceId();
  if( destId )
  {
    mResourceClient.ClearTexture( destId, color );
    ThreadLocalStorage::Get().GetImageFactory().ClearAtlasResourceCache( destId );
  }
}

bool Atlas::Upload( const BufferImage& bufferImage,
                    std::size_t xOffset,
                    std::size_t yOffset )
{
  bool uploadSuccess( false );

  if( Compatible(bufferImage.GetPixelFormat(),
                 xOffset + bufferImage.GetWidth(),
                 yOffset + bufferImage.GetHeight() ) )
  {
    AllocateAtlas();
    ResourceId destId = GetResourceId();
    ResourceId srcId = bufferImage.GetResourceId();

    if( destId && srcId )
    {
      mResourceClient.UploadBitmap( destId, srcId, xOffset, yOffset );
      uploadSuccess = true;
    }
  }

  return uploadSuccess;
}

bool Atlas::Upload( const std::string& url,
                    std::size_t xOffset,
                    std::size_t yOffset)
{
  bool uploadSuccess( false );

  ImageFactory& imageFactory = ThreadLocalStorage::Get().GetImageFactory();
  ResourceId destId = GetResourceId();

  if( destId )
  {
    ImageFactoryCache::AtlasResourceCache* resourceCache = imageFactory.RegisterAtlasResource( destId, url, xOffset, yOffset  );
    if( resourceCache )
    {
      Integration::BitmapPtr bitmap = resourceCache->bitmap;

      if( Compatible(bitmap->GetPixelFormat(), xOffset + bitmap->GetImageWidth(), yOffset + bitmap->GetImageHeight()) )
      {
        AllocateAtlas();
        imageFactory.UploadBitmapToAtlas( *resourceCache  );
        uploadSuccess = true;
      }
    }
  }

  return uploadSuccess;
}

Atlas::~Atlas()
{
  ReleaseAtlas();
}

Atlas::Atlas( std::size_t width,
              std::size_t height,
              Pixel::Format pixelFormat )
: mResourceClient( ThreadLocalStorage::Get().GetResourceClient() ),
  mPixelFormat( pixelFormat )
{
  mWidth  = width;
  mHeight = height;
}

void Atlas::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    AllocateAtlas();
  }
}

void Atlas::Disconnect()
{
  if( mConnectionCount )
  {
    --mConnectionCount;

    if ( Dali::Image::UNUSED == mReleasePolicy &&
         mConnectionCount == 0 )
    {
      ReleaseAtlas();
    }
  }
}

bool Atlas::Compatible( Pixel::Format pixelFormat,
                        std::size_t x,
                        std::size_t y )
{
  bool Compatible(false);

  if( mPixelFormat != pixelFormat )
  {
    DALI_LOG_ERROR( "Pixel format %d does not match Atlas format %d\n", pixelFormat, mPixelFormat );
  }
  else
  {
    if( x <= mWidth  && y <= mHeight )
    {
      Compatible = true;
    }
    else
    {
      DALI_LOG_ERROR( "image does not fit within the atlas \n" );
    }
  }

  return Compatible;
}

void Atlas::AllocateAtlas()
{
  if( !mTicket )
  {
    mTicket = mResourceClient.AllocateTexture( mWidth, mHeight, mPixelFormat );
  }
}

void Atlas::ReleaseAtlas()
{
  mTicket.Reset();
}

} // namespace Internal

} // namespace Dali
