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
#include <dali/public-api/images/n-patch-image.h>

// INTERNAL INCLUDES
#include <dali/internal/common/image-attributes.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/images/n-patch-image-impl.h>


namespace Dali
{

NPatchImage::NPatchImage()
{
}

NPatchImage::NPatchImage(Internal::NPatchImage* internal)
: ResourceImage(internal)
{
}

NPatchImage::~NPatchImage()
{
}

NPatchImage::NPatchImage(const NPatchImage& handle)
: ResourceImage(handle)
{
}

NPatchImage& NPatchImage::operator=(const NPatchImage& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

NPatchImage NPatchImage::New( const std::string& filename )
{
  Internal::ImageAttributes defaultAttrs;

  Internal::NPatchImagePtr internal = Internal::NPatchImage::New( filename, defaultAttrs, Image::NEVER );
  return NPatchImage(internal.Get());
}

NPatchImage NPatchImage::DownCast( BaseHandle handle )
{
  return NPatchImage( dynamic_cast<Dali::Internal::NPatchImage*>(handle.GetObjectPtr()) );
}

const NPatchImage::StretchRanges& NPatchImage::GetStretchPixelsX()
{
  return GetImplementation(*this).GetStretchPixelsX();
}
const NPatchImage::StretchRanges& NPatchImage::GetStretchPixelsY()
{
  return GetImplementation(*this).GetStretchPixelsY();
}

Rect<int> NPatchImage::GetChildRectangle()
{
  return GetImplementation(*this).GetChildRectangle();
}

BufferImage NPatchImage::CreateCroppedBufferImage()
{
  Internal::BufferImagePtr internal = GetImplementation(*this).CreateCroppedBufferImage();
  return BufferImage(internal.Get());
}


} // namespace Dali
