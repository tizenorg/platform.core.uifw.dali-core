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
#include <dali/public-api/images/native-image.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/native-image-impl.h>

namespace Dali
{

NativeImage::NativeImage()
{
}

NativeImage::NativeImage(Internal::NativeImage* internal)
: Image(internal)
{
}

NativeImage::~NativeImage()
{
}

NativeImage::NativeImage( const NativeImage& handle )
: Image(handle)
{
}

NativeImage& NativeImage::operator=( const NativeImage& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

void NativeImage::CreateGlTexture()
{
  GetImplementation(*this).CreateGlTexture();
}

NativeImage NativeImage::New( NativeImageInterface& resourceData )
{
  Internal::NativeImagePtr internal = Internal::NativeImage::New( resourceData );
  return NativeImage(internal.Get());
}

NativeImage NativeImage::DownCast( BaseHandle handle )
{
  return NativeImage( dynamic_cast<Internal::NativeImage*>( handle.GetObjectPtr()) );
}

void NativeImage::GlContextCreated()
{
  // Derived classes on particular platforms may respond to this but
  // here we don't care.
}

} // namespace Dali
