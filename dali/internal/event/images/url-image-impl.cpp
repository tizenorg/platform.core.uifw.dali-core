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
#include <dali/internal/event/images/url-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>
#include <dali/internal/event/common/stage-impl.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{

BaseHandle CreateImage()
{
  ImagePtr image = UrlImage::New();
  return Dali::Image(image.Get());
}

TypeRegistration mType( typeid(Dali::UrlImage), typeid(Dali::Image), CreateImage );

Dali::SignalConnectorType signalConnector1(mType, Dali::UrlImage::SIGNAL_IMAGE_LOADING_FINISHED, &UrlImage::DoConnectSignal);

}

UrlImage::Image( LoadPolicy loadPol, ReleasePolicy releasePol )
: mImageFactory(ThreadLocalStorage::Get().GetImageFactory()),
  mWidth(0),
  mHeight(0),
  mConnectionCount(0),
  mLoadPolicy(loadPol),
  mReleasePolicy(releasePol)
{
}

UrlImage* UrlImage::New()
{
  UrlImage* image = new UrlImage;
  image->Initialize();
  return image;
}

ImagePtr UrlImage::New( const std::string& url, const Dali::ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  ImagePtr image;
  if( IsNinePatchUrl( url ) )
  {
    image = NinePatchUrlImage::New( url, attributes, loadPol, releasePol );
  }
  else
  {
    image = new UrlImage( loadPol, releasePol );
    image->Initialize();

    // consider the requested size as natural size, 0 means we don't (yet) know it
    image->mWidth = attributes.GetWidth();
    image->mHeight = attributes.GetHeight();
    image->mRequest = image->mImageFactory.RegisterRequest( url, &attributes );

    if( Dali::UrlImage::Immediate == loadPol )
    {
      // Trigger loading of the image on a as soon as it can be done
      image->mTicket = image->mImageFactory.Load( *image->mRequest.Get() );
      image->mTicket->AddObserver( *image );
    }
  }
  DALI_LOG_SET_OBJECT_STRING( image, url );

  return image;
}

UrlImage::~Image()
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

bool UrlImage::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  DALI_ASSERT_DEBUG( dynamic_cast<Image*>( object ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  Image* image = static_cast<Image*>(object);

  if( Dali::UrlImage::SIGNAL_IMAGE_LOADING_FINISHED == signalName )
  {
    image->LoadingFinishedSignal().Connect( tracker, functor );
  }
  else if(Dali::UrlImage::SIGNAL_IMAGE_UPLOADED == signalName)
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

const Dali::ImageAttributes& UrlImage::GetAttributes() const
{
  if( mTicket )
  {
    return mImageFactory.GetActualAttributes( mTicket );
  }
  else
  {
    return mImageFactory.GetRequestAttributes( mRequest );
  }
}

const std::string& UrlImage::GetFilename() const
{
  return mImageFactory.GetRequestPath( mRequest );
}

void UrlImage::Reload()
{
  if ( mRequest )
  {
    ResourceTicketPtr ticket = mImageFactory.Reload( *mRequest.Get() );
    SetTicket( ticket.Get() );
  }
}

void UrlImage::ResourceLoadingFailed(const ResourceTicket& ticket)
{
  mLoadingFinishedV2.Emit( Dali::Image( this ) );
}

void UrlImage::ResourceLoadingSucceeded(const ResourceTicket& ticket)
{
  mLoadingFinishedV2.Emit( Dali::Image( this ) );
}

unsigned int UrlImage::GetWidth() const
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

unsigned int UrlImage::GetHeight() const
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

Vector2 UrlImage::GetNaturalSize() const
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

void UrlImage::Connect()
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

void UrlImage::Disconnect()
{
  if( !mTicket )
  {
    return;
  }

  DALI_ASSERT_DEBUG( mConnectionCount > 0 );
  --mConnectionCount;
  if( mConnectionCount == 0 && mReleasePolicy == Dali::UrlImage::Unused )
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    SetTicket( NULL );
  }
}

void UrlImage::Initialize()
{
  RegisterObject();
}

void UrlImage::SetTicket( ResourceTicket* ticket )
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

bool UrlImage::IsNinePatchFileName( const std::string& filename )
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

    // Satisfy prevent
    if( state == DONE )
    {
      break;
    }

    ++iter;
  }
  return match;
}


} // namespace Internal

} // namespace Dali
