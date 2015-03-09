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
#include <dali/internal/event/effects/sampler-impl.h> // Dali::Internal::Sampler

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/sampler.h>     // Dali::Internal::Sampler
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

/**
 *            |name                    |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "minification-filter",   STRING,   true, false,  true, Dali::Sampler::Property::MINIFICATION_FILTER )
DALI_PROPERTY( "magnification-filter",  STRING,   true, false,  true, Dali::Sampler::Property::MAGNIGICATION_FILTER )
DALI_PROPERTY( "u-wrap",                STRING,   true, false,  true, Dali::Sampler::Property::U_WRAP )
DALI_PROPERTY( "v-wrap",                STRING,   true, false,  true, Dali::Sampler::Property::V_WRAP )
DALI_PROPERTY( "affects-transparency",  BOOLEAN,  true, false,  true, Dali::Sampler::Property::AFFECTS_TRANSPARENCY )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )


SamplerPtr Sampler::New()
{
  return SamplerPtr( new Sampler() );
}


unsigned int Sampler::GetDefaultPropertyCount() const
{
  //TODO: MESH_REWORK
  return 0;
}

void Sampler::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  //TODO: MESH_REWORK
}

const char* Sampler::GetDefaultPropertyName(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Index Sampler::GetDefaultPropertyIndex(const std::string& name) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Sampler::IsDefaultPropertyWritable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Sampler::IsDefaultPropertyAnimatable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Sampler::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Type Sampler::GetDefaultPropertyType(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

void Sampler::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  //TODO: MESH_REWORK
}

void Sampler::SetSceneGraphProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  //TODO: MESH_REWORK
}

Property::Value Sampler::GetDefaultProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Sampler::GetPropertyOwner() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Sampler::GetSceneObject() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyBase* Sampler::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const PropertyInputImpl* Sampler::GetSceneObjectInputProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

int Sampler::GetPropertyComponentIndex( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

} // namespace Internal
} // namespace Dali

