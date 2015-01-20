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
#include <dali/internal/event/images/image-factory.h>
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

UrlImage::UrlImage( LoadPolicy loadPol, ReleasePolicy releasePol )
: Image( releasePol ),
  mLoadPolicy(loadPol)
{
}

UrlImagePtr UrlImage::New()
{
  UrlImagePtr image = new UrlImage;
  image->Initialize();
  return image;
}

UrlImagePtr UrlImage::New( const std::string& url, const Dali::ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  UrlImagePtr image;
  if( IsNinePatch( url ) )
  {
    image = NinePatchImage::New( url, attributes, releasePol );
  }
  else
  {
    image = new UrlImage( loadPol, releasePol );
    image->Initialize();

    // consider the requested size as natural size, 0 means we don't (yet) know it
    image->mWidth = attributes.GetWidth();
    image->mHeight = attributes.GetHeight();
    image->mRequest = image->mImageFactory.RegisterRequest( url, &attributes );

    if( Dali::UrlImage::IMMEDIATE == loadPol )
    {
      // Trigger loading of the image on a as soon as it can be done
      image->mTicket = image->mImageFactory.Load( *image->mRequest.Get() );
      image->mTicket->AddObserver( *image );
    }
  }
  DALI_LOG_SET_OBJECT_STRING( image, url );

  return image;
}

UrlImage::~UrlImage()
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
  DALI_ASSERT_DEBUG( dynamic_cast<UrlImage*>( object ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  UrlImage* image = static_cast<UrlImage*>(object);

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

const std::string& UrlImage::GetUrl() const
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
