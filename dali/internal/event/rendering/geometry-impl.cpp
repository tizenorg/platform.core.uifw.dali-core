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
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                    |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "geometry-type",         STRING,   true, false, true, Dali::Geometry::Property::GEOMETRY_TYPE )
DALI_PROPERTY( "requires-depth-test",   BOOLEAN,  true, false, true, Dali::Geometry::Property::REQUIRES_DEPTH_TEST )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> GEOMETRY_IMPL = { DEFAULT_PROPERTY_DETAILS };

BaseHandle Create()
{
  return Dali::Geometry::New();
}

TypeRegistration mType( typeid( Dali::Geometry ), typeid( Dali::Handle ), Create );

} // unnamed namespace

GeometryPtr Geometry::New()
{
  GeometryPtr geometry( new Geometry() );
  geometry->Initialize();
  return geometry;
}

std::size_t Geometry::AddVertexBuffer( PropertyBuffer& vertexBuffer )
{
  mVertexBuffers.push_back( &vertexBuffer );
  SceneGraph::AddVertexBufferMessage( GetEventThreadServices(), *mSceneObject, *vertexBuffer.GetRenderObject() );
  return mVertexBuffers.size() - 1u;
}

std::size_t Geometry::GetNumberOfVertexBuffers() const
{
  return mVertexBuffers.size();
}

void Geometry::RemoveVertexBuffer( std::size_t index )
{
  const Render::PropertyBuffer& renderPropertyBuffer = static_cast<const Render::PropertyBuffer&>( *(mVertexBuffers[index]->GetRenderObject()) );
  SceneGraph::RemoveVertexBufferMessage( GetEventThreadServices(), *mSceneObject, renderPropertyBuffer );

  mVertexBuffers.erase( mVertexBuffers.begin() + index );
}

void Geometry::SetIndexBuffer( PropertyBuffer& indexBuffer )
{
  mIndexBuffer = &indexBuffer;
  SceneGraph::SetIndexBufferMessage( GetEventThreadServices(), *mSceneObject, *indexBuffer.GetRenderObject() );
}

void Geometry::SetGeometryType( Dali::Geometry::GeometryType geometryType )
{
  if( geometryType != mGeometryType )
  {
    SceneGraph::SetGeometryTypeMessage(GetEventThreadServices(),
                                       *mSceneObject,
                                       geometryType );

    mGeometryType = geometryType;
  }
}

Dali::Geometry::GeometryType Geometry::GetGeometryType() const
{
  return mGeometryType;
}

void Geometry::SetRequiresDepthTesting( bool requiresDepthTest )
{
  if( requiresDepthTest != mRequiresDepthTest )
  {
    SceneGraph::SetGeometryRequiresDepthTestMessage(GetEventThreadServices(),
                                                    *mSceneObject,
                                                    requiresDepthTest );

    mRequiresDepthTest = requiresDepthTest;
  }
}

bool Geometry::GetRequiresDepthTesting() const
{
  return mRequiresDepthTest;
}

const SceneGraph::Geometry* Geometry::GetGeometrySceneObject() const
{
  return mSceneObject;
}

unsigned int Geometry::GetDefaultPropertyCount() const
{
  return GEOMETRY_IMPL.GetDefaultPropertyCount();
}

void Geometry::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  GEOMETRY_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Geometry::GetDefaultPropertyName(Property::Index index) const
{
  return GEOMETRY_IMPL.GetDefaultPropertyName( index );
}

Property::Index Geometry::GetDefaultPropertyIndex( const std::string& name ) const
{
  return GEOMETRY_IMPL.GetDefaultPropertyIndex( name );
}

bool Geometry::IsDefaultPropertyWritable( Property::Index index ) const
{
  return GEOMETRY_IMPL.IsDefaultPropertyWritable( index );
}

bool Geometry::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return GEOMETRY_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Geometry::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return GEOMETRY_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Geometry::GetDefaultPropertyType( Property::Index index ) const
{
  return GEOMETRY_IMPL.GetDefaultPropertyType( index );
}

void Geometry::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  switch( index )
  {
    case Dali::Geometry::Property::GEOMETRY_TYPE :
    {
      Dali::Geometry::GeometryType geometryType = static_cast<Dali::Geometry::GeometryType>(propertyValue.Get<int>());
      if( geometryType != mGeometryType )
      {
        SceneGraph::SetGeometryTypeMessage(GetEventThreadServices(), *mSceneObject, geometryType );
        mGeometryType = geometryType;
      }
      break;
    }
    case Dali::Geometry::Property::REQUIRES_DEPTH_TEST :
    {
      bool requiresDepthTest = propertyValue.Get<bool>();
      if( requiresDepthTest != mRequiresDepthTest )
      {
        SceneGraph::SetGeometryRequiresDepthTestMessage(GetEventThreadServices(), *mSceneObject, requiresDepthTest);
        mRequiresDepthTest = requiresDepthTest;
      }
      break;
    }
  }
}

void Geometry::SetSceneGraphProperty( Property::Index index,
                                      const PropertyMetadata& entry,
                                      const Property::Value& value )
{
  GEOMETRY_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
}

Property::Value Geometry::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch( index )
  {
    case Dali::Geometry::Property::GEOMETRY_TYPE :
    {
      if( mSceneObject )
      {
        value = mGeometryType;
      }
      break;
    }
    case Dali::Geometry::Property::REQUIRES_DEPTH_TEST :
    {
      if( mSceneObject )
      {
        value = mRequiresDepthTest;
      }
      break;
    }
  }

  return value;
}

const SceneGraph::PropertyOwner* Geometry::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Geometry::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Geometry::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  const SceneGraph::PropertyBase* property = NULL;
  if( OnStage() )
  {
    property = GEOMETRY_IMPL.GetRegisteredSceneGraphProperty ( this,
                                                               &Geometry::FindAnimatableProperty,
                                                               &Geometry::FindCustomProperty,
                                                               index );
  }

  return property;
}

const PropertyInputImpl* Geometry::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  if( OnStage() )
  {
    const SceneGraph::PropertyBase* baseProperty =
        GEOMETRY_IMPL.GetRegisteredSceneGraphProperty ( this,
                                                        &Geometry::FindAnimatableProperty,
                                                        &Geometry::FindCustomProperty,
                                                        index );

    property = static_cast<const PropertyInputImpl*>( baseProperty );
  }

  return property;
}

bool Geometry::OnStage() const
{
  return mOnStage;
}

void Geometry::Connect()
{
  mOnStage = true;
}

void Geometry::Disconnect()
{
  mOnStage = false;
}

Geometry::Geometry()
: mSceneObject( NULL ),
  mIndexBuffer( NULL ),
  mGeometryType(Dali::Geometry::TRIANGLES),
  mRequiresDepthTest(false),
  mOnStage( false )
{
}

void Geometry::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = new SceneGraph::Geometry();
  AddMessage( updateManager, updateManager.GetGeometryOwner(), *mSceneObject );

  eventThreadServices.RegisterObject( this );
}

Geometry::~Geometry()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetGeometryOwner(), *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}


} // namespace Internal
} // namespace Dali
