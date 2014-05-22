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

#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/images/image-factory.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>
#include <dali/internal/event/common/stage-impl.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

Image::Image( LoadPolicy loadPol, ReleasePolicy releasePol )
: mWidth(0),
  mHeight(0),
  mLoadPolicy(loadPol),
  mReleasePolicy(releasePol),
  mConnectionCount(0),
  mImageFactory(ThreadLocalStorage::Get().GetImageFactory())
{
}

Image* Image::New()
{
  return new Image;
}

Image* Image::New( const std::string& filename, const Dali::ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  if( IsNinePatchFileName(filename) )
  {
    NinePatchImage* image = new NinePatchImage( filename, attributes, loadPol, releasePol );
    return image;
  }
  else
  {
    Image* image = new Image( loadPol, releasePol );

    if( ! filename.empty() )
    {
      Vector2 closestSize;

      Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().GetClosestImageSize( filename, attributes, closestSize );
      image->mWidth = closestSize.width;
      image->mHeight = closestSize.height;
    }

    image->mRequest = image->mImageFactory.RegisterRequest( filename, &attributes );

    if( Dali::Image::Immediate == loadPol )
    {
      // Trigger loading of the image on a seperate resource thread as soon as it
      // can be scheduled:
      image->mTicket = image->mImageFactory.Load( image->mRequest.Get() );
      image->mTicket->AddObserver( *image );
    }
    // else lazily load image data later, only when it is needed to draw something:

    DALI_LOG_SET_OBJECT_STRING( image, filename );

    return image;
  }
}

Image* Image::New( NativeImage& nativeImg, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  Image* image = new Image;
  ResourceClient &resourceClient = ThreadLocalStorage::Get().GetResourceClient();


  image->mWidth  = nativeImg.GetWidth();
  image->mHeight = nativeImg.GetHeight();

  const ResourceTicketPtr& ticket = resourceClient.AddNativeImage( nativeImg );
  DALI_ASSERT_DEBUG( dynamic_cast<ImageTicket*>( ticket.Get() ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  image->mTicket = static_cast<ImageTicket*>(ticket.Get());
  image->mTicket->AddObserver( *image );

  return image;
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

const Dali::ImageAttributes& Image::GetAttributes() const
{
  if( mTicket )
  {
    return mImageFactory.GetActualAttributes( mTicket->GetId() );
  }
  else
  {
    return mImageFactory.GetRequestAttributes( mRequest.Get() );
  }
}

const std::string& Image::GetFilename() const
{
  return mImageFactory.GetRequestPath( mRequest.Get() );
}

void Image::Reload()
{
  if ( mRequest )
  {
    ResourceTicketPtr ticket = mImageFactory.Reload( mRequest.Get() );
    SetTicket( ticket.Get() );
  }
}

void Image::ResourceLoadingFailed(const ResourceTicket& ticket)
{
  mLoadingFinishedV2.Emit( Dali::Image( this ) );
}

void Image::ResourceLoadingSucceeded(const ResourceTicket& ticket)
{
  // Update size with actual loaded size
  const ImageTicket* imageTicket = static_cast<const ImageTicket*>(&ticket);
  mWidth = imageTicket->GetWidth();
  mHeight = imageTicket->GetHeight();
  mLoadingFinishedV2.Emit( Dali::Image( this ) );
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
  // Width has already been calculated - just return that
  return mWidth;
}

unsigned int Image::GetHeight() const
{
  // Height has already been calculated - just return that
  return mHeight;
}

Vector2 Image::GetNaturalSize() const
{
  return Vector2( mWidth, mHeight );
}

void Image::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    // ticket was thrown away when related actors went offstage or image loading on demand
    if( !mTicket )
    {
      ResourceTicketPtr newTicket = mImageFactory.Load( mRequest.Get() );
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
  if( mConnectionCount == 0 && mReleasePolicy == Dali::Image::Unused )
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    SetTicket( NULL );
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

bool Image::IsNinePatchFileName( std::string filename )
{
  bool match = false;

  std::string::const_reverse_iterator iter = filename.rbegin();
  enum { SUFFIX, HASH, HASH_DOT, DONE } state = SUFFIX;
  while(iter < filename.rend())
  {
    switch(state)
    {
      case SUFFIX:
      {
        if(*iter == '.')
        {
          state = HASH;
        }
        else if(!isalnum(*iter))
        {
          state = DONE;
        }
      }
      break;
      case HASH:
      {
        if( *iter == '#' || *iter == '9' )
        {
          state = HASH_DOT;
        }
        else
        {
          state = DONE;
        }
      }
      break;
      case HASH_DOT:
      {
        if(*iter == '.')
        {
          match = true;
        }
        state = DONE; // Stop testing characters
      }
      break;
      case DONE:
      {
      }
      break;
    }

    // Satisfy prevnet
    if( state == DONE )
    {
      break;
    }

    iter++;
  }
  return match;
}


} // namespace Internal

} // namespace Dali
