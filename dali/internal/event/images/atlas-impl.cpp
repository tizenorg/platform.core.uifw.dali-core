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

Atlas* Atlas::New( unsigned int width,
                   unsigned int height,
                   Pixel::Format pixelFormat )
{
  return new Atlas( width, height, pixelFormat );
}

void Atlas::Clear(const Vector4& color)
{
  ClearCache();
  ClearBackground( color );
}

bool Atlas::Upload( BufferImage& bufferImage,
                    unsigned int xOffset,
                    unsigned int yOffset )
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
      unsigned int index = 0;
      // avoid recording the same buffer image again
      for( std::vector<BufferImagePtr>::iterator iter = mBufferImages.begin();
           iter != mBufferImages.end(); iter++, index++)
      {
        if( *(iter) == &bufferImage )
        {
          break;
        }
      }
      if( index ==  mBufferImages.size())
      {
        mBufferImages.push_back( &bufferImage );
      }

      mTiles.push_back( new Tile(xOffset, yOffset, index, true) );

      mResourceClient.UploadBitmap( destId, srcId, xOffset, yOffset );
      uploadSuccess = true;
    }
  }

  return uploadSuccess;
}

bool Atlas::Upload( const std::string& url,
                    unsigned int xOffset,
                    unsigned int yOffset)
{
  bool uploadSuccess( false );

  ResourceId destId = GetResourceId();

  if( destId )
  {
    unsigned int index = 0;
    bool addNewUrl(false);
    // avoid recording the same url resource again
    for( std::vector<UrlResource*>::iterator iter = mUrlResources.begin();
        iter != mUrlResources.end(); iter++, index++)
    {
      if( url.compare((*iter)->url) == 0 )
      {
        break;
      }
    }
    if( index ==  mUrlResources.size())
    {
      mUrlResources.push_back( new UrlResource(url) );
      mUrlResources[index]->LoadBitmap();
      addNewUrl = true;
    }

    Integration::BitmapPtr bitmap = mUrlResources[index]->bitmap;
    if( bitmap && Compatible(bitmap->GetPixelFormat(), xOffset + bitmap->GetImageWidth(), yOffset + bitmap->GetImageHeight()) )
    {
      AllocateAtlas();
      mTiles.push_back( new Tile(xOffset, yOffset, index, false) );
      mResourceClient.UploadBitmap( destId, bitmap, xOffset, yOffset  );
      uploadSuccess = true;
    }
    // If the bitmap loaded from the url is failed to upload, delete the url resource we newly added
    else if( addNewUrl )
    {
      delete mUrlResources[index];
      mUrlResources.pop_back();
    }
  }

  return uploadSuccess;
}

void Atlas::RecoverFromContextLoss()
{
  ResourceId destId = GetResourceId();
  if( destId )
  {
    if( mClear )
    {
      ClearBackground( mClearColor );
    }

    // Restore the atlas following the uploading order
    for( std::vector<Tile*>::iterator iter = mTiles.begin(); iter != mTiles.end(); iter++ )
    {
      if( (*iter)->isBufferImage )
      {
        ResourceId srcId = mBufferImages[(*iter)->sourceIndex]->GetResourceId();
        mResourceClient.UploadBitmap( destId, srcId, (*iter)->xOffset, (*iter)->yOffset );
      }
      else
      {
        UrlResource* urlResource = mUrlResources[ (*iter)->sourceIndex ];
        mResourceClient.UploadBitmap( destId, urlResource->bitmap, (*iter)->xOffset, (*iter)->yOffset  );
      }
    }

   }
}

Atlas::~Atlas()
{
  ReleaseAtlas();
}

Atlas::Atlas( unsigned int width,
              unsigned int height,
              Pixel::Format pixelFormat )
: mResourceClient( ThreadLocalStorage::Get().GetResourceClient() ),
  mClearColor( Vector4::ZERO ),
  mPixelFormat( pixelFormat ),
  mClear( false )
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
                        unsigned int x,
                        unsigned int y )
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
    ThreadLocalStorage::Get().GetImageFactory().RegisterForContextRecovery( this );
  }
}

void Atlas::ReleaseAtlas()
{
  mTicket.Reset();
  ClearCache();
  ThreadLocalStorage::Get().GetImageFactory().UnregisterFromContextRecovery( this );
}

void Atlas::ClearBackground(const Vector4& color )
{
  AllocateAtlas();
  ResourceId destId = GetResourceId();
  if( destId )
  {
    const unsigned int numPixels = mWidth * mHeight;
    unsigned int bytesPerPixel = Pixel::GetBytesPerPixel(mPixelFormat);
    BufferImagePtr imageData = BufferImage::New( mWidth, mHeight, mPixelFormat );
    PixelBuffer* pixbuf = imageData->GetBuffer();

    uint32_t clearColor;
    unsigned char r =  0xFF * color.r;
    unsigned char g =  0xFF * color.g;
    unsigned char b =  0xFF * color.b;
    unsigned char a =  0xFF * color.a;
    if( mPixelFormat == Pixel::RGBA8888 )
    {
      clearColor = ( (uint32_t) a<<24 | (uint32_t)b << 16 | (uint32_t)g << 8 | (uint32_t)r );
    }
    else if( mPixelFormat == Pixel::RGB888 )
    {
      clearColor = ( (uint32_t)b << 16 | (uint32_t)g << 8 | (uint32_t)r);
    }
    else if( mPixelFormat == Pixel::A8 )
    {
      clearColor = (uint32_t)a;
    }

    for( unsigned int i = 0; i < numPixels; ++i )
    {
      memcpy(&pixbuf[i*bytesPerPixel], &clearColor, bytesPerPixel);
    }

    RectArea area;
    imageData->Update(area);

    mClearColor = color;
    mClear = true;
    mResourceClient.UploadBitmap( destId, imageData->GetResourceId(), 0, 0 );
  }
}

void Atlas::ClearCache()
{
  for( std::vector<Tile*>::iterator iter = mTiles.begin(); iter != mTiles.end(); iter++ )
  {
    delete *iter;
  }
  mTiles.clear();

  for( std::vector<UrlResource*>::iterator iter = mUrlResources.begin();
            iter != mUrlResources.end(); iter++)
  {
    delete *iter;
  }
  mUrlResources.clear();

  mBufferImages.clear();
}

void Atlas::UrlResource::LoadBitmap()
{
  ImageAttributes loadedAttrs;
  Integration::BitmapResourceType resourceType( loadedAttrs );
  Integration::PlatformAbstraction& platformAbstraction = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();

  Integration::ResourcePointer resource = platformAbstraction.LoadResourceSynchronously(resourceType, url);
  bitmap = static_cast<Integration::Bitmap*>( resource.Get());
}

} // namespace Internal

} // namespace Dali
