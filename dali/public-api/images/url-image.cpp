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
#include <dali/public-api/images/image.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/image-attributes.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/images/url-image-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

const char* const UrlImage::SIGNAL_IMAGE_LOADING_FINISHED = "image-loading-finished";

Vector2 UrlImage::GetImageSize( const std::string& url )
{
  Vector2 size;
  Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().GetClosestImageSize( url, ImageAttributes::DEFAULT_ATTRIBUTES, size );
  return size;
}

UrlImage::UrlImage()
{
}

UrlImage::UrlImage(Internal::UrlImage* internal) : Image(internal)
{
}

UrlImage::~UrlImage()
{
}

UrlImage::UrlImage( const UrlImage& handle )
: Image(handle)
{
}

UrlImage& UrlImage::operator=( const UrlImage& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

UrlImage UrlImage::New( const std::string& url )
{
  Internal::UrlImagePtr internal = Internal::UrlImage::New( url,
                                                         Dali::ImageAttributes::DEFAULT_ATTRIBUTES );
  return UrlImage(internal.Get());
}

UrlImage UrlImage::New( const std::string& url, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  Internal::UrlImagePtr internal = Internal::UrlImage::New( url,
                                                         Dali::ImageAttributes::DEFAULT_ATTRIBUTES,
                                                         loadPol, releasePol );
  return UrlImage(internal.Get());
}

UrlImage UrlImage::New( const std::string& url, const ImageAttributes& attributes )
{
  Internal::UrlImagePtr internal = Internal::UrlImage::New( url, attributes );
  return UrlImage(internal.Get());
}

UrlImage UrlImage::New( const std::string& url, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  Internal::UrlImagePtr internal = Internal::UrlImage::New( url, attributes, loadPol, releasePol );
  return UrlImage(internal.Get());
}

UrlImage UrlImage::DownCast( BaseHandle handle )
{
  return UrlImage( dynamic_cast<Dali::Internal::UrlImage*>(handle.GetObjectPtr()) );
}

UrlImage::LoadPolicy UrlImage::GetLoadPolicy() const
{
  return GetImplementation(*this).GetLoadPolicy();
}

LoadingState UrlImage::GetLoadingState() const
{
  return GetImplementation(*this).GetLoadingState();
}

std::string UrlImage::GetUrl() const
{
  return GetImplementation(*this).GetUrl();
}

void UrlImage::Reload()
{
  GetImplementation(*this).Reload();
}

ImageAttributes UrlImage::GetAttributes() const
{
  return GetImplementation(*this).GetAttributes();
}

UrlImage::UrlImageSignal& UrlImage::LoadingFinishedSignal()
{
  return GetImplementation(*this).LoadingFinishedSignal();
}

} // namespace Dali
