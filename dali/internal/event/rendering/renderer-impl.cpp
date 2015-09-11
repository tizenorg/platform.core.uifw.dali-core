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
#include <dali/internal/event/rendering/renderer-impl.h> // Dali::Internal::Renderer

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/rendering/renderer.h> // Dali::Renderer

#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/node-attachments/scene-graph-renderer-attachment.h>
#include <dali/internal/update/rendering/scene-graph-renderer2.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name          |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "depth-index", INTEGER, true, false, false, Dali::Renderer::Property::DEPTH_INDEX )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> RENDERER_IMPL = { DEFAULT_PROPERTY_DETAILS };

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Renderer ), typeid( Dali::Handle ), Create );

} // unnamed namespace

RendererPtr Renderer::New()
{
  std::cout<<"Creating event thread renderer"<<std::endl;
  RendererPtr rendererPtr( new Renderer() );
  rendererPtr->Initialize();
  return rendererPtr;
}

void Renderer::SetGeometry( Geometry& geometry )
{
  mGeometryConnector.Set( geometry, OnStage() );
  const SceneGraph::Geometry* geometrySceneObject = geometry.GetGeometrySceneObject();

  SetGeometryMessage( GetEventThreadServices(), *mSceneObject, *geometrySceneObject );
}

Geometry* Renderer::GetGeometry() const
{
  return mGeometryConnector.Get().Get();
}

void Renderer::SetMaterial( Material& material )
{
  mMaterialConnector.Set( material, OnStage() );
  const SceneGraph::Material* materialSceneObject = material.GetMaterialSceneObject();
  SetMaterialMessage( GetEventThreadServices(), *mSceneObject, *materialSceneObject );
}

Material* Renderer::GetMaterial() const
{
  return mMaterialConnector.Get().Get();
}

void Renderer::SetDepthIndex( int depthIndex )
{
  if ( mDepthIndex != depthIndex )
  {
    mDepthIndex = depthIndex;
    SetDepthIndexMessage( GetEventThreadServices(), *mSceneObject, depthIndex );
  }
}

int Renderer::GetDepthIndex() const
{
  return mDepthIndex;
}

SceneGraph::R3nderer* Renderer::GetRendererSceneObject()
{
  return mSceneObject;
}

unsigned int Renderer::GetDefaultPropertyCount() const
{
  return RENDERER_IMPL.GetDefaultPropertyCount();
}

void Renderer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  RENDERER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Renderer::GetDefaultPropertyName(Property::Index index) const
{
  return RENDERER_IMPL.GetDefaultPropertyName( index );
}

Property::Index Renderer::GetDefaultPropertyIndex( const std::string& name ) const
{
  return RENDERER_IMPL.GetDefaultPropertyIndex( name );
}

bool Renderer::IsDefaultPropertyWritable( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyWritable( index );
}

bool Renderer::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Renderer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Renderer::GetDefaultPropertyType( Property::Index index ) const
{
  return RENDERER_IMPL.GetDefaultPropertyType( index );
}

void Renderer::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  switch( index )
  {
    case Dali::Renderer::Property::DEPTH_INDEX:
    {
      SetDepthIndex( propertyValue.Get<int>() );
    }
    break;
  }
}

void Renderer::SetSceneGraphProperty( Property::Index index,
                                      const PropertyMetadata& entry,
                                      const Property::Value& value )
{
  //@FERRAN

  //RENDERER_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  //OnPropertySet(index, value);
}

Property::Value Renderer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;
  switch( index )
  {
    case Dali::Renderer::Property::DEPTH_INDEX:
    {
      value = GetDepthIndex();
    }
    break;
  }
  return value;
}

const SceneGraph::PropertyOwner* Renderer::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Renderer::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Renderer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );
  const SceneGraph::PropertyBase* property = NULL;

  if( OnStage() )
  {
    property = RENDERER_IMPL.GetRegisteredSceneGraphProperty(
      this,
      &Renderer::FindAnimatableProperty,
      &Renderer::FindCustomProperty,
      index );
  }

  return property;
}

const PropertyInputImpl* Renderer::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  if( OnStage() )
  {
    const SceneGraph::PropertyBase* baseProperty =
      RENDERER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                     &Renderer::FindAnimatableProperty,
                                                     &Renderer::FindCustomProperty,
                                                     index );
    property = static_cast<const PropertyInputImpl*>( baseProperty );
  }

  return property;
}

int Renderer::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool Renderer::OnStage() const
{
  return mOnStage;
}

void Renderer::Connect()
{

}

void Renderer::Disconnect()
{

}

void Renderer::Connect(const SceneGraph::Node* node)
{
  // @todo: MESH_REWORK : check this
  //mSceneObject->OnStageConnect(node);
  OnStageConnectMessage( GetEventThreadServices(), *mSceneObject, node );
  mGeometryConnector.OnStageConnect();
  mMaterialConnector.OnStageConnect();
  mOnStage = true;
}

void Renderer::Disconnect(const SceneGraph::Node* node)
{
  // @todo: MESH_REWORK : check this
  mSceneObject->OnStageDisconnect(node);
  mGeometryConnector.OnStageDisconnect();
  mMaterialConnector.OnStageDisconnect();
  mOnStage = false;
}

Renderer::Renderer()
: mSceneObject(NULL),
  mDepthIndex(0),
  mOnStage(false)
{
}

void Renderer::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = new SceneGraph::R3nderer();
  AddMessage( updateManager, updateManager.GetRendererOwner(), *mSceneObject );

  eventThreadServices.RegisterObject( this );
//  EventThreadServices& eventThreadServices = GetEventThreadServices();
//  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
//
//  // Transfer object ownership of scene-object to message
//  mSceneObject = SceneGraph::RendererAttachment::New();
//
//  // Send message to update to connect to scene graph:
//  AttachToSceneGraphMessage( updateManager, mSceneObject );
//
//  eventThreadServices.RegisterObject( this );
}

Renderer::~Renderer()
{
  std::cout<<"Destroying event thread renderer"<<std::endl;
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetRendererOwner(), *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}

} // namespace Internal
} // namespace Dali
