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
 */

// CLASS HEADER
#include "scene-graph-geometry.h"

// INTERNAL HEADERS
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Geometry::Geometry()
: mIndexBuffer( NULL ),
  mRenderGeometry(0),
  mSceneController(0),
  mRendererRefCount(0u),
  mCenter(),
  mGeometryType(Dali::Geometry::TRIANGLES),
  mRequiresDepthTest(false)
{

  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
}

Geometry::~Geometry()
{
  if( mRenderGeometry )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveGeometry( *mRenderGeometry );
  }
  mConnectionObservers.Destroy( *this );
}

void Geometry::AddVertexBuffer( Render::PropertyBuffer* vertexBuffer )
{
  mVertexBuffers.PushBack( vertexBuffer );
  CalculateExtents( vertexBuffer );
  mConnectionObservers.ConnectionsChanged(*this);

  if( mRenderGeometry )
  {
    mSceneController->GetRenderMessageDispatcher().AddPropertyBuffer( *mRenderGeometry, vertexBuffer, false);
  }
}

void Geometry::RemoveVertexBuffer( Render::PropertyBuffer* vertexBuffer )
{
  DALI_ASSERT_DEBUG( NULL != vertexBuffer );

  // Find the object and destroy it
  //FERRAN: TODO remove find
  Vector<Render::PropertyBuffer*>::Iterator match = std::find(
    mVertexBuffers.Begin(),
    mVertexBuffers.End(),
    vertexBuffer );

  DALI_ASSERT_DEBUG( mVertexBuffers.End() != match );
  if( mVertexBuffers.End() != match )
  {
    mVertexBuffers.Erase( match );
    mConnectionObservers.ConnectionsChanged(*this);

    if( mRenderGeometry )
    {
      mSceneController->GetRenderMessageDispatcher().RemovePropertyBuffer( *mRenderGeometry, vertexBuffer );
    }
  }
}

void Geometry::SetIndexBuffer( Render::PropertyBuffer* indexBuffer )
{
  if( mIndexBuffer != indexBuffer )
  {
    mIndexBuffer = indexBuffer;
    mConnectionObservers.ConnectionsChanged(*this);

    if( mRenderGeometry )
    {
      mSceneController->GetRenderMessageDispatcher().AddPropertyBuffer( *mRenderGeometry, indexBuffer, true );
    }
  }
}

void Geometry::ClearIndexBuffer()
{
  if( mIndexBuffer )
  {
    if( mRenderGeometry )
    {
      mSceneController->GetRenderMessageDispatcher().RemovePropertyBuffer( *mRenderGeometry, mIndexBuffer );
    }
  }
  mIndexBuffer = 0;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Geometry::SetGeometryType( BufferIndex bufferIndex, Geometry::GeometryType geometryType )
{
  mGeometryType.Set( bufferIndex, geometryType);
}

Vector<Render::PropertyBuffer*>& Geometry::GetVertexBuffers()
{
  return mVertexBuffers;
}

Render::PropertyBuffer* Geometry::GetIndexBuffer()
{
  return mIndexBuffer;
}

Geometry::GeometryType Geometry::GetGeometryType( BufferIndex bufferIndex) const
{
  int geometryType = mGeometryType[ bufferIndex ];
  return static_cast< GeometryDataProvider::GeometryType > ( geometryType );
}

bool Geometry::GetRequiresDepthTesting( BufferIndex bufferIndex ) const
{
  return mRequiresDepthTest.GetBoolean( bufferIndex );
}

void Geometry::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  // Reset the animated properties
  mCenter.ResetToBaseValue( updateBufferIndex );

  // Age the double buffered properties
  mGeometryType.CopyPrevious(updateBufferIndex);
  mRequiresDepthTest.CopyPrevious(updateBufferIndex);
}

void Geometry::CalculateExtents( Render::PropertyBuffer* vertexBuffer )
{
  //FERRAN :Todo
//  unsigned int elementIndex = 0;
//  unsigned int elementCount = vertexBuffer->GetElementCount( 0 );
//  unsigned int elementCount1 = vertexBuffer->GetElementCount( 1 );
//
//  // Select the double buffered element list that is the largest...
//  if ( elementCount < elementCount1 )
//  {
//    elementCount = elementCount1;
//    elementIndex = 1;
//  }
//
//  unsigned int attributeCount = vertexBuffer->GetAttributeCount( elementIndex );
//  unsigned int elementSize = vertexBuffer->GetElementSize( elementIndex );
//
//  std::string posName( "aPos" );
//  std::size_t found;
//
//  float left = 0.0f;
//  float right = 0.0f;
//  float top = 0.0f;
//  float bottom = 0.0f;
//
//  // Find the position attribute index
//  for ( unsigned int i = 0; i < attributeCount; ++i )
//  {
//    found = vertexBuffer->GetAttributeName( 0, i ).find( posName );
//    if ( found != std::string::npos )
//    {
//      unsigned int offset = vertexBuffer->GetAttributeOffset( elementIndex, i );
//      const PropertyBufferDataProvider::BufferType& data = vertexBuffer->GetData( elementIndex );
//
//      // Check attribute type to determine correct position type
//      Property::Type positionType = vertexBuffer->GetAttributeType( elementIndex, i );
//      Vector3 halfExtents;
//      Vector3 center;
//      switch ( positionType )
//      {
//        case Property::VECTOR2:
//        {
//          for ( unsigned int j = 0; j < elementCount; ++j )
//          {
//            const Vector2* position = reinterpret_cast< const Vector2* >( &data[ offset ] );
//            offset += elementSize;
//
//            if ( position->x < left )
//            {
//              left = position->x;
//            }
//            if ( position->x > right )
//            {
//              right = position->x;
//            }
//            if ( position->y < top )
//            {
//              top = position->y;
//            }
//            if ( position->y > bottom )
//            {
//              bottom = position->y;
//            }
//          }
//
//          halfExtents = Vector3( ( right - left ) * 0.5f, ( bottom - top ) * 0.5f, 0.0f );
//          center = Vector3( halfExtents.x + left , halfExtents.y + top, 0.0f );
//          break;
//        }
//        case Property::VECTOR3:
//        {
//          float near = 0.0f;
//          float far = 0.0f;
//          for ( unsigned int j = 0; j < elementCount; ++j )
//          {
//            const Vector3* position = reinterpret_cast< const Vector3* >( &data[ offset ] );
//            offset += elementSize;
//
//            if ( position->x < left )
//            {
//              left = position->x;
//            }
//            if ( position->x > right )
//            {
//              right = position->x;
//            }
//            if ( position->y < top )
//            {
//              top = position->y;
//            }
//            if ( position->y > bottom )
//            {
//              bottom = position->y;
//            }
//            if ( position->z > far )
//            {
//              far = position->z;
//            }
//            if ( position->z < near )
//            {
//              near = position->z;
//            }
//          }
//          halfExtents = Vector3( ( right - left ) * 0.5f, ( bottom - top ) * 0.5f, ( far - near ) * 0.5f );
//          center = Vector3( halfExtents.x + left , halfExtents.y + top, halfExtents.z + near );
//          break;
//        }
//        default:
//        {
//          break;
//        }
//      }
//      mCenter.Bake( 0, center );
//      mCenter.Bake( 1, center );
//    }
//  }
}

void Geometry::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Geometry::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Geometry::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Geometry::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Geometry::UniformMappingsChanged( const UniformMap& mappings )
{
  // Our uniform map, or that of one of the watched children has changed.
  // Inform connected observers.
  mConnectionObservers.ConnectedUniformMapChanged();
}

RenderGeometry* Geometry::GetRenderGeometry(SceneController* sceneController)
{
  if(!mRenderGeometry )
  {
    //Create RenderGeometry
    mSceneController = sceneController;
    mRenderGeometry = new RenderGeometry( *this );

    size_t vertexBufferCount( mVertexBuffers.Size() );
    for( size_t i(0); i<vertexBufferCount; ++i )
    {
      mRenderGeometry->AddPropertyBuffer( mVertexBuffers[i], false );
    }

    if( mIndexBuffer )
    {
      mRenderGeometry->AddPropertyBuffer( mIndexBuffer, true );
    }

    //Transfer ownership to RenderManager
    sceneController->GetRenderMessageDispatcher().AddGeometry( *mRenderGeometry );
  }

  ++mRendererRefCount;
  return mRenderGeometry;
}

void Geometry::OnRendererDisconnect()
{
  --mRendererRefCount;
  if( mRendererRefCount == 0 && mRenderGeometry)
  {
    //Delete the corresponding RenderGeometry via message to RenderManager
    mSceneController->GetRenderMessageDispatcher().RemoveGeometry( *mRenderGeometry );

    mRenderGeometry = 0;
    mSceneController = 0;
  }
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
