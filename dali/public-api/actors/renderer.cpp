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
#include <dali/public-api/actors/renderer.h>  // Dali::Renderer

// INTERNAL INCLUDES
#include <dali/internal/event/actors/renderer-impl.h> // Dali::Internal::Renderer

namespace Dali
{

Renderer Renderer::New( Geometry geometry, Material material )
{
  Internal::RendererPtr renderer = Internal::Renderer::New(  );
  return Renderer( renderer.Get() );
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

Renderer::Renderer( const Renderer& handle )
: Handle( handle )
{
}

Renderer& Renderer::operator=( const Renderer& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

void Renderer::SetGeometry( Geometry geometry )
{
  GetImplementation(*this).SetGeometry( GetImplementation(geometry) );
}

void Renderer::SetMaterial( Material material )
{
  GetImplementation(*this).SetMaterial( GetImplementation(material) );
}

void Renderer::SetDepthIndex( int depthIndex )
{
  GetImplementation(*this).SetDepthIndex( depthIndex );
}

Renderer::Renderer( Internal::Renderer* pointer )
: Handle( pointer )
{
}

} //namespace Dali
