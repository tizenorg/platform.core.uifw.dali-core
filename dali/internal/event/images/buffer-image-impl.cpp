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
#include <dali/internal/event/images/buffer-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/images/image-factory.h>

// EXTERNAL INCLUDES
#include <cstring>

using Dali::Integration::Bitmap;

namespace Dali
{
namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::BufferImage ), typeid( Dali::Image ), NULL );

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_BUFFER_IMAGE" );
#endif

Integration::BitmapPtr CloneBitmap( Integration::BitmapPtr bitmap )
{
  DALI_ASSERT_ALWAYS( bitmap );

  Bitmap::PackedPixelsProfile* const bitmapProfile = bitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(bitmapProfile);

  unsigned int  width       = bitmapProfile->GetBufferWidth();
  unsigned int  height      = bitmapProfile->GetBufferHeight();
  Pixel::Format pixelFormat = bitmap->GetPixelFormat();

  Integration::BitmapPtr clonedBitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD );
  Bitmap::PackedPixelsProfile* const clonedProfile = clonedBitmap->GetPackedPixelsProfile();
  clonedProfile->ReserveBuffer( pixelFormat, width, height, width, height );

  DALI_ASSERT_DEBUG(bitmap->GetBuffer() != 0);
  DALI_ASSERT_DEBUG(bitmap->GetBufferSize() >= width * height);
  DALI_ASSERT_DEBUG(clonedBitmap->GetBuffer() != 0);

  DALI_LOG_INFO( gLogFilter, Debug::General, "CloneBitmap() src:%p dest:%p size:%u\n",
                 bitmap->GetBuffer(),
                 clonedBitmap->GetBuffer(),
                 bitmap->GetBufferSize() );

  memcpy( clonedBitmap->GetBuffer(), bitmap->GetBuffer(), bitmap->GetBufferSize() );
  return clonedBitmap ;
}

} // unnamed namespace

BufferImagePtr BufferImage::New(
  unsigned int  width,
  unsigned int  height,
  Pixel::Format pixelformat )
{
  BufferImagePtr internal = new BufferImage( NULL, width, height, pixelformat, 0u );
  internal->Initialize();
  RegisterImage( internal.Get() );
  return internal;
}

BufferImagePtr BufferImage::New(
  PixelBuffer*  pixBuf,
  unsigned int  width,
  unsigned int  height,
  Pixel::Format pixelformat,
  unsigned int  stride )
{
  BufferImagePtr internal = new BufferImage( pixBuf, width, height, pixelformat, stride );
  internal->Initialize();
  RegisterImage( internal.Get() );
  return internal;
}

BufferImage::BufferImage(
  PixelBuffer*  pixBuf,
  unsigned int  width,
  unsigned int  height,
  Pixel::Format pixelFormat,
  unsigned int  stride )
: Image( Dali::Image::UNUSED )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;

  // This bitmap is owned by the event object, and will not get discarded.
  mBitmap = CreateBitmap( pixBuf, width, height, pixelFormat, stride, ResourcePolicy::OWNED_RETAIN );
}

BufferImage::~BufferImage()
{
  DeregisterImage(this);
}

void BufferImage::Update( RectArea& updateArea )
{
  if( mTicket)
  {
    DALI_ASSERT_DEBUG( updateArea.x + updateArea.width <= mWidth && updateArea.y + updateArea.height <= mHeight );

    if( updateArea.IsEmpty() )
    {
      UploadBitmap( mTicket->GetId(), 0, 0 );
    }
    else
    {
      Integration::Bitmap::PackedPixelsProfile* profile = mBitmap->GetPackedPixelsProfile();

      Integration::BitmapPtr smallBitmap = CreateBitmap( NULL, updateArea.width, updateArea.height, mBitmap->GetPixelFormat(), profile->GetBufferStride(), ResourcePolicy::OWNED_DISCARD );

      DALI_ASSERT_DEBUG(smallBitmap->GetBufferSize() >= updateArea.width * updateArea.height * Pixel::GetBytesPerPixel( smallBitmap->GetPixelFormat() ) );

      CopyPixelArea( smallBitmap->GetBuffer(), updateArea );
      mResourceClient->UploadBitmap( mTicket->GetId(), smallBitmap, updateArea.x, updateArea.y );
    }
  }
  else
  {
    CreateTicket(); // Will automatically clone the bitmap data and ignore the update area
  }
}

PixelBuffer* BufferImage::GetBuffer()
{
  DALI_ASSERT_DEBUG( mBitmap );
  return mBitmap->GetBuffer();
}

unsigned int BufferImage::GetBufferSize() const
{
  DALI_ASSERT_DEBUG( mBitmap );
  return mBitmap->GetBufferSize();
}

unsigned int BufferImage::GetBufferStride() const
{
  DALI_ASSERT_DEBUG( mBitmap );
  Integration::Bitmap::PackedPixelsProfile* packedBitmap = mBitmap->GetPackedPixelsProfile();

  DALI_ASSERT_DEBUG(packedBitmap);
  return packedBitmap->GetBufferStride();
}

Pixel::Format BufferImage::GetPixelFormat() const
{
  DALI_ASSERT_DEBUG( mBitmap );

  return mBitmap->GetPixelFormat();
}

void BufferImage::UploadBitmap( ResourceId destId, std::size_t xOffset, std::size_t yOffset )
{
  DALI_ASSERT_DEBUG( mBitmap );

  Integration::BitmapPtr clonedBitmap = CloneBitmap( mBitmap );
  mResourceClient->UploadBitmap( destId, clonedBitmap, xOffset, yOffset);
}

void BufferImage::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    if( !mTicket )
    {
      CreateTicket();
    }
  }
}

void BufferImage::Disconnect()
{
  if( !mTicket)
  {
    return;
  }

  --mConnectionCount;

  if( mConnectionCount == 0 )
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    mTicket->RemoveObserver(*this);
    mTicket.Reset();
  }
}

Integration::BitmapPtr BufferImage::CreateBitmap(
  PixelBuffer* pixBuf,
  unsigned int width,
  unsigned int height,
  Pixel::Format pixelFormat,
  unsigned int stride,
  ResourcePolicy::Discardable discardPolicy )
{
  Integration::BitmapPtr bitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, discardPolicy );
  Bitmap::PackedPixelsProfile* const packedBitmap = bitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(packedBitmap);

  packedBitmap->ReserveBuffer( pixelFormat, width, height, width, height);
  DALI_ASSERT_DEBUG(bitmap->GetBuffer() != 0);
  DALI_ASSERT_DEBUG(bitmap->GetBufferSize() >= width * height);

  DALI_LOG_INFO(gLogFilter, Debug::General, "BufferImage::CreateBitmap(pixbuf:%p, w:%d,h:%d stride:%d)\n", pixBuf, width, height, stride);

  if( pixBuf != NULL )
  {
    if( stride == packedBitmap->GetBufferStride() || stride == 0 )
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "  Copying complete pixel buffer\n");
      memcpy( bitmap->GetBuffer(), pixBuf, bitmap->GetBufferSize() );
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "  Copying pixel buffer\n");

      uint32_t bytesPerPixel = Pixel::GetBytesPerPixel( pixelFormat );
      uint32_t srcStride = stride * bytesPerPixel;
      uint32_t destStride = width * bytesPerPixel;
      uint32_t destWidth = std::min( srcStride, destStride );
      PixelBuffer* src = pixBuf;
      PixelBuffer* dest = bitmap->GetBuffer();

      for( uint32_t i = 0; i < height; ++i )
      {
        memcpy( dest, src, destWidth );
        src += srcStride;
        dest += destStride;
      }
    }
  }
  return bitmap;
}


void BufferImage::CreateTicket()
{
  Integration::BitmapPtr clonedBitmap = CloneBitmap( mBitmap );
  mTicket.Reset( mResourceClient->AddBitmapImage( clonedBitmap.Get() ).Get() );
  mTicket->AddObserver(*this);
}

void BufferImage::CopyPixelArea( PixelBuffer* dest, const RectArea& area )
{
  DALI_ASSERT_DEBUG( area.x + area.width <= mWidth && area.y + area.height <= mHeight );

  Integration::Bitmap::PackedPixelsProfile* profile = mBitmap->GetPackedPixelsProfile();
  uint32_t bytesPerPixel = Pixel::GetBytesPerPixel( mBitmap->GetPixelFormat() );
  uint32_t srcWidth = profile->GetBufferWidth();
  uint32_t srcStride = srcWidth * bytesPerPixel;
  uint32_t destStride = area.width * bytesPerPixel;

  PixelBuffer* src = mBitmap->GetBuffer();
  src += ( area.y * srcStride ) + ( area.x * bytesPerPixel );

  for( uint32_t i=0; i < area.height; ++i )
  {
    memcpy( dest, src, destStride );
    src += srcStride;
    dest += destStride;
  }
}

Dali::Vector<BufferImage*> BufferImage::gRegisteredImages;

void BufferImage::RegisterImage( BufferImage* bufferImage )
{
  gRegisteredImages.PushBack(bufferImage);
}

void BufferImage::DeregisterImage( BufferImage* bufferImage )
{
  Dali::Vector<BufferImage*>::Iterator iter = std::find( gRegisteredImages.Begin(), gRegisteredImages.End(), bufferImage);
  if( iter != gRegisteredImages.End() )
  {
    gRegisteredImages.Erase( iter );
  }
}

void BufferImage::RecoverFromContextLoss()
{
  for( Dali::Vector<BufferImage*>::Iterator iter = gRegisteredImages.Begin() ;
       iter != gRegisteredImages.End() ;
       ++iter )
  {
    BufferImage* image = *iter;
    DALI_ASSERT_DEBUG( image->ReferenceCount() > 0 );

    if( image->mConnectionCount > 0 )
    {
      RectArea empty;
      image->Update( empty );
    }
  }
}

} // namespace Internal

} // namespace Dali
