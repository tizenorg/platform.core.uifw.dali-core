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

// CLASS HEADER
#include <dali/internal/event/images/image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>

#include <dali/integration-api/debug.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/images/image-factory.h>
#include <dali/internal/event/common/stage-impl.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{

TypeRegistration mType( typeid(Dali::Image), typeid(Dali::BaseHandle), NULL );

Dali::SignalConnectorType signalConnector1(mType, Dali::Image::SIGNAL_IMAGE_LOADING_FINISHED,    &Image::DoConnectSignal);
Dali::SignalConnectorType signalConnector2(mType, Dali::Image::SIGNAL_IMAGE_UPLOADED,            &Image::DoConnectSignal);

}

bool Image::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  DALI_ASSERT_DEBUG( dynamic_cast<Image*>( object ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  Image* image = static_cast<Image*>(object);

  if( Dali::Image::SIGNAL_IMAGE_LOADING_FINISHED == signalName )
  {
    image->LoadingFinishedSignal().Connect( tracker, functor );
  }
  else if(Dali::Image::SIGNAL_IMAGE_UPLOADED == signalName)
  {
    image->UploadedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

ResourceId Image::GetResourceId() const
{
  ResourceId ret = mTicket ? mTicket->GetId() : 0;

  return ret;
}

void Image::ResourceUploaded(const ResourceTicket& ticket)
{
  mUploadedV2.Emit( Dali::Image( this ) );
}

void Image::ResourceSavingSucceeded( const ResourceTicket& ticket )
{
  // do nothing
}

void Image::ResourceSavingFailed( const ResourceTicket& ticket )
{
  // do nothing
}

unsigned int Image::GetWidth() const
{
  // if width is 0, it means we've not yet loaded the image
  if( 0u == mWidth )
  {
    Size size;
    mImageFactory.GetImageSize( mRequest, mTicket, size );
    mWidth = size.width;
    // The app will probably ask for the height immediately, so don't waste the synchronous file IO that ImageFactory may have just done:
    DALI_ASSERT_DEBUG( 0 == mHeight || unsigned(size.height) == mHeight );
    if( 0 == mHeight )
    {
      mHeight = size.height;
    }
  }
  return mWidth;
}

unsigned int Image::GetHeight() const
{
  if( 0u == mHeight )
  {
    Size size;
    mImageFactory.GetImageSize( mRequest, mTicket, size );
    mHeight = size.height;
    DALI_ASSERT_DEBUG( 0 == mWidth || unsigned(size.width) == mWidth );
    if( 0 == mWidth )
    {
      mWidth = size.width;
    }
  }
  return mHeight;
}

Vector2 Image::GetNaturalSize() const
{
  Vector2 naturalSize(mWidth, mHeight);
  if( 0u == mWidth || 0u == mHeight )
  {
    mImageFactory.GetImageSize( mRequest, mTicket, naturalSize );
    mWidth = naturalSize.width;
    mHeight = naturalSize.height;
  }
  return naturalSize;
}

void Image::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    // ticket was thrown away when related actors went offstage or image loading on demand
    if( !mTicket )
    {
      DALI_ASSERT_DEBUG( mRequest.Get() );
      ResourceTicketPtr newTicket = mImageFactory.Load( *mRequest.Get() );
      SetTicket( newTicket.Get() );
    }
  }
}

void Image::Disconnect()
{
  if( !mTicket )
  {
    return;
  }

  DALI_ASSERT_DEBUG( mConnectionCount > 0 );
  --mConnectionCount;
  if( mConnectionCount == 0 && mReleasePolicy == Dali::Image::UNUSED )
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    SetTicket( NULL );
  }
}

Image::Image( ReleasePolicy releasePol )
: mImageFactory( ThreadLocalStorage::Get().GetImageFactory() ),
  mWidth( 0 ),
  mHeight( 0 ),
  mConnectionCount( 0 ),
  mReleasePolicy( releasePol )
{
}

Image::~Image()
{
  if( mTicket )
  {
    mTicket->RemoveObserver( *this );
    if( Stage::IsInstalled() )
    {
      mImageFactory.ReleaseTicket( mTicket.Get() );
    }
  }

  if( Stage::IsInstalled() )
  {
    UnregisterObject();
  }
}

void Image::SetTicket( ResourceTicket* ticket )
{
  if( ticket == mTicket.Get() )
  {
    return;
  }

  if( mTicket )
  {
    mTicket->RemoveObserver( *this );
    mImageFactory.ReleaseTicket( mTicket.Get() );
  }

  if( ticket )
  {
    mTicket.Reset( ticket );
    mTicket->AddObserver( *this );
  }
  else
  {
    mTicket.Reset();
  }
}

} // namespace Internal

} // namespace Dali
