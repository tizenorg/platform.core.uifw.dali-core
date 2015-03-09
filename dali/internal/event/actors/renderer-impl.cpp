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
#include <dali/internal/event/actors/renderer-impl.h> // Dali::Internal::Renderer

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderer.h> // Dali::Renderer
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

// SIZE = DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX, ///< name "size", type UNSIGNED_INT

/**
 *            |name          |type             |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "depth-index", INTEGER, true, true, true, Dali::Renderer::Property::DEPTH_INDEX )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )



RendererPtr Renderer::New()
{
  return RendererPtr( new Renderer() );
}


unsigned int Renderer::GetDefaultPropertyCount() const
{
  //TODO: MESH_REWORK
  return 0;
}

void Renderer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  //TODO: MESH_REWORK
}

const char* Renderer::GetDefaultPropertyName(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Index Renderer::GetDefaultPropertyIndex(const std::string& name) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Renderer::IsDefaultPropertyWritable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Renderer::IsDefaultPropertyAnimatable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Renderer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Type Renderer::GetDefaultPropertyType(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

void Renderer::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  //TODO: MESH_REWORK
}

void Renderer::SetSceneGraphProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  //TODO: MESH_REWORK
}

Property::Value Renderer::GetDefaultProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Renderer::GetPropertyOwner() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Renderer::GetSceneObject() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyBase* Renderer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const PropertyInputImpl* Renderer::GetSceneObjectInputProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

int Renderer::GetPropertyComponentIndex( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

} // namespace Internal
} // namespace Dali

