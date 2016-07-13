/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use *this file except in compliance with the License.
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
#include <dali/public-api/rendering/geometry.h>  // Dali::Geometry

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry

namespace Dali
{

Geometry Geometry::New()
{
  Internal::GeometryPtr geometry = Internal::Geometry::New();
  return Geometry( geometry.Get() );
}

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

Geometry::Geometry( const Geometry& handle )
: BaseHandle( handle )
{
}

Geometry Geometry::DownCast( BaseHandle handle )
{
  return Geometry( dynamic_cast<Dali::Internal::Geometry*>(handle.GetObjectPtr()));
}

Geometry& Geometry::operator=( const Geometry& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

std::size_t Geometry::AddVertexBuffer( PropertyBuffer& vertexBuffer )
{
  DALI_ASSERT_ALWAYS( vertexBuffer && "VertexBuffer is not initialized ");
  return GetImplementation(*this).AddVertexBuffer( GetImplementation( vertexBuffer ) );
}

std::size_t Geometry::GetNumberOfVertexBuffers() const
{
  return GetImplementation(*this).GetNumberOfVertexBuffers();
}

void Geometry::RemoveVertexBuffer( std::size_t index )
{
  GetImplementation(*this).RemoveVertexBuffer( index );
}

void Geometry::SetIndexBuffer( const unsigned short* indices, size_t count )
{
  GetImplementation(*this).SetIndexBuffer( indices, count );
}

void Geometry::SetGeometryType( GeometryType geometryType )
{
  GetImplementation(*this).SetGeometryType( geometryType );
}

Geometry::GeometryType Geometry::GetGeometryType() const
{
  return GetImplementation(*this).GetGeometryType();
}

Geometry Geometry::QUAD()
{
  Dali::Property::Map quadVertexFormat;
  quadVertexFormat["aPosition"] = Dali::Property::VECTOR2;
  Dali::PropertyBuffer vertexData = Dali::PropertyBuffer::New( quadVertexFormat );

  const float halfQuadSize = .5f;
  struct QuadVertex { Dali::Vector2 position; };
  QuadVertex quadVertexData[4] = {
      { Dali::Vector2(-halfQuadSize, -halfQuadSize) },
      { Dali::Vector2(-halfQuadSize, halfQuadSize) },
      { Dali::Vector2( halfQuadSize, -halfQuadSize) },
      { Dali::Vector2( halfQuadSize, halfQuadSize) } };
  vertexData.SetData(quadVertexData, 4);

  Dali::Geometry quad = Dali::Geometry::New();
  quad.AddVertexBuffer( vertexData );
  quad.SetGeometryType( Dali::Geometry::TRIANGLE_STRIP );
  return quad;
}

Geometry::Geometry( Internal::Geometry* pointer )
: BaseHandle( pointer )
{
}

} //namespace Dali
