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

// EXTERNAL INCLUDES
#include <string.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/images/image-factory.h>

using namespace Dali::Integration;

namespace Dali
{
namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::BufferImage ), typeid( Dali::Image ), NULL );
} // unnamed namespace

BufferImagePtr BufferImage::New( unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePol )
{
  BufferImagePtr internal = new BufferImage( width, height, pixelformat, releasePol );
  internal->Initialize();
  return internal;
}

BufferImagePtr BufferImage::New( PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol )
{
  BufferImagePtr internal = new BufferImage( pixBuf, width, height, pixelformat, stride, releasePol );
  internal->Initialize();
  return internal;
}

BufferImage::BufferImage(unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePol)
: Image(releasePol),
  mExternalBuffer(NULL),
  mBitmap(NULL)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  mPixelFormat = pixelformat;
  mBytesPerPixel = Pixel::GetBytesPerPixel( pixelformat );
  mByteStride = width * mBytesPerPixel;
  mBufferSize = height * mByteStride;

  // Respect the desired release policy
  mResourcePolicy = releasePol == Dali::Image::UNUSED ? ResourcePolicy::DISCARD : ResourcePolicy::RETAIN;
}

BufferImage::BufferImage(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol )
: Image(releasePol),
  mExternalBuffer(pixBuf),
  mBitmap(NULL)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  mPixelFormat = pixelformat;
  mBytesPerPixel = Pixel::GetBytesPerPixel( pixelformat );
  mByteStride = ( stride ? stride : mWidth ) * mBytesPerPixel;
  mBufferSize = height * mByteStride;

  // Always discard on bitmap upload as we update from an external source
  mResourcePolicy = ResourcePolicy::DISCARD;

  // Take a copy of the external buffer immediately, so it can be released if desired
  RectArea area;
  MirrorExternal( area );
}

BufferImage::~BufferImage()
{
}

bool BufferImage::IsDataExternal() const
{
  return ( mExternalBuffer ? true : false );
}

void BufferImage::Update( RectArea& updateArea )
{
  if (mExternalBuffer)
  {
    MirrorExternal( updateArea );
  }
  else
  {
    GetBuffer();
  }

  if (mTicket)
  {
    DALI_ASSERT_DEBUG( updateArea.x + updateArea.width <= mWidth && updateArea.y + updateArea.height <= mHeight );
    mResourceClient->UpdateBitmapArea( mTicket, updateArea );

    if ( mResourcePolicy == ResourcePolicy::DISCARD )
    {
      // Bitmap ownership has been passed on, so any subsequent update will need another bitmap
      mBitmap = NULL;
    }
  }
}

PixelBuffer* BufferImage::GetBuffer()
{
  PixelBuffer* buffer = NULL;
  {
    // Does a bitmap currently exist ?
    if ( !mBitmap )
    {
      // No so create a new one...
      mBitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, mResourcePolicy );
      mTicket = mResourceClient->AddBitmapImage( mBitmap );
      mTicket->AddObserver(*this);
    }

    // Do we have a pixel buffer?
    buffer = mBitmap->GetBuffer();
    if ( !buffer )
    {
      Bitmap::PackedPixelsProfile* const packedBitmap = mBitmap->GetPackedPixelsProfile();
      DALI_ASSERT_DEBUG(packedBitmap);

      buffer = packedBitmap->ReserveBuffer( mPixelFormat, mWidth, mHeight, mByteStride / mBytesPerPixel, mHeight );
      DALI_ASSERT_DEBUG(mBitmap->GetBuffer() != 0);
      DALI_ASSERT_DEBUG(mBitmap->GetBufferSize() >= mWidth * mHeight * Pixel::GetBytesPerPixel( mBitmap->GetPixelFormat() ) );
    }
  }

  // If we're using an external buffer then we return that as the writable buffer
  if ( mExternalBuffer )
  {
    buffer = mExternalBuffer;
  }
  return buffer;
}

void BufferImage::UpdateBufferArea( PixelBuffer* src, const RectArea& area )
{
  DALI_ASSERT_DEBUG( area.x + area.width <= mWidth && area.y + area.height <= mHeight );

  PixelBuffer* dest = GetBuffer();
  uint32_t width = area.width * mBytesPerPixel;
  uint32_t stride = mWidth * mBytesPerPixel;

  src += ( area.y * mByteStride ) + ( area.x * mBytesPerPixel );
  dest +=( ( area.y * mWidth ) + area.x ) * mBytesPerPixel;
  for ( uint32_t i = 0; i < area.height; ++i )
  {
    memcpy( dest, src, width );
    src += mByteStride;
    dest += stride;
  }
}

void BufferImage::MirrorExternal( const RectArea& area )
{
  if( ( mByteStride == mWidth * mBytesPerPixel ) && area.IsEmpty() )
  {
    memcpy( GetBuffer(), mExternalBuffer, mBufferSize );
  }
  else
  {
    UpdateBufferArea( mExternalBuffer, area );
  }
}

void BufferImage::Connect()
{
  mConnectionCount++;

  // Connected to stage, so make sure we have a bitmap and a ticket by calling GetBuffer()
  GetBuffer();

}

void BufferImage::Disconnect()
{
  if ( mTicket )
  {
    if ( !( --mConnectionCount ) && mReleasePolicy == Dali::Image::UNUSED )
    {
      mTicket->RemoveObserver(*this);
      mTicket.Reset();
    }
  }
}

} // namespace Internal

} // namespace Dali
