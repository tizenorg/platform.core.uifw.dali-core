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

namespace Dali
{
namespace Internal
{

using namespace Dali::Integration;

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
  mExternalBuffer(NULL)

{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();

  mWidth  = width;
  mHeight = height;

  const ImageTicketPtr& t = AllocateBitmapImage( width, height, width, height, pixelformat );
  mTicket = t.Get();
  mTicket->AddObserver(*this);
}

BufferImage::BufferImage(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol )
: Image(releasePol),
  mExternalBuffer(pixBuf)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();

  mWidth  = width;
  mHeight = height;
  mPixelStride = stride;

  const ImageTicketPtr& t = AllocateBitmapImage( width, height, stride ? stride : width, height, pixelformat );
  mTicket = t.Get();
  mTicket->AddObserver(*this);

   // Mirror source buffer
  memcpy( GetBuffer(), pixBuf, GetBufferSize() );
}

BufferImage::~BufferImage()
{
}

ImageTicketPtr BufferImage::AllocateBitmapImage( uint32_t width,
                                                 uint32_t height,
                                                 uint32_t bufferWidth,
                                                 uint32_t bufferHeight,
                                                 Pixel::Format pixelformat )
{
  mBitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  Bitmap::PackedPixelsProfile* const packedBitmap = mBitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(packedBitmap);

  packedBitmap->ReserveBuffer(pixelformat, width, height, bufferWidth, bufferHeight);
  DALI_ASSERT_DEBUG(mBitmap->GetBuffer() != 0);
  DALI_ASSERT_DEBUG(mBitmap->GetBufferSize() >= width * height);

  ImageTicketPtr ticket = mResourceClient->AddBitmapImage(mBitmap);
  return ticket;
}

bool BufferImage::IsDataExternal() const
{
  return ( mExternalBuffer ? true : false );
}

void BufferImage::Update( RectArea& updateArea )
{
  if (mExternalBuffer)
  {
    // Make sure any external buffers update the internal buffer
    if( !mPixelStride && updateArea.IsEmpty() )
    {
      memcpy( GetBuffer(), mExternalBuffer, GetBufferSize() );
    }
    else
    {

      DALI_ASSERT_DEBUG( updateArea.x + updateArea.width <= mWidth && updateArea.y + updateArea.height <= mHeight );

      PixelBuffer* src = mExternalBuffer;
      PixelBuffer* dest = GetBuffer();
      uint32_t byteStride = GetBufferStride();
      uint32_t offset = ( updateArea.y * byteStride ) + ( updateArea.x * mBytesPerPixel );
      uint32_t width = updateArea.width * mBytesPerPixel;

      src += offset;
      dest += offset;
      for ( uint32_t i = 0; i < updateArea.height; ++i )
      {
        memcpy( dest, src, width );
        src += byteStride;
        dest += byteStride;
      }
    }
  }

  if (mTicket)
  {
    DALI_ASSERT_DEBUG( updateArea.x + updateArea.width <= mWidth && updateArea.y + updateArea.height <= mHeight );
    mResourceClient->UpdateBitmapArea( mTicket, updateArea );
  }
}

unsigned int BufferImage::GetBufferStride() const
{
  Integration::Bitmap::PackedPixelsProfile* packedBitmap = mBitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(packedBitmap);
  return packedBitmap->GetBufferStride();
}

void BufferImage::Connect()
{
  ++mConnectionCount;

  if (mConnectionCount == 1)
  {
    if (!mTicket)
    {
      const ImageTicketPtr& t = mResourceClient->AddBitmapImage( mBitmap );
      mTicket = t.Get();
    }
  }
}

void BufferImage::Disconnect()
{
  if (!mTicket)
  {
    return;
  }

  --mConnectionCount;

  if (mConnectionCount == 0 && mReleasePolicy == Dali::Image::UNUSED)
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    mTicket.Reset();
  }
}

} // namespace Internal

} // namespace Dali
