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
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/common/thread-local-storage.h>

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

bool Atlas::Upload( const BitmapImage& bitmap,
                    std::size_t xOffset,
                    std::size_t yOffset )
{
  // TODO
  return false;
}

Atlas::~Atlas()
{
}

Atlas::Atlas( std::size_t width,
              std::size_t height,
              Pixel::Format pixelformat )
: mPixelformat( pixelformat )
{
  mWidth  = width;
  mHeight = height;

  if( Dali::Image::Immediate == mLoadPolicy )
  {
    AllocateAtlas();
  }
}

void Atlas::Connect()
{
  ++mConnectionCount;

  if( Dali::Image::OnDemand == mLoadPolicy &&
      mConnectionCount == 1 )
  {
    AllocateAtlas();
  }
}

void Atlas::Disconnect()
{
  if( mConnectionCount )
  {
    --mConnectionCount;

    if ( Dali::Image::Unused == mReleasePolicy &&
         mConnectionCount == 0 )
    {
      ReleaseAtlas();
    }
  }
}

void Atlas::AllocateAtlas()
{
  if( !mTicket )
  {
    ResourceClient& resourceClient = ThreadLocalStorage::Get().GetResourceClient();

    mTicket = resourceClient.AllocateBitmapImage( mWidth, mHeight, mWidth, mHeight, mPixelformat );
  }
}

void Atlas::ReleaseAtlas()
{
  // TODO
}

} // namespace Internal

} // namespace Dali
