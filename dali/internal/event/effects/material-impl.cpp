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
#include <dali/internal/event/effects/material-impl.h> // Dali::Internal::Material

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/material.h> // Dali::Internal::Material
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                              |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "color",                           VECTOR4,  true, true,   true, Dali::Material::Property::COLOR )
DALI_PROPERTY( "face-culling-mode",               STRING,   true, false,  true, Dali::Material::Property::FACE_CULLING_MODE )
DALI_PROPERTY( "blending-mode",                   STRING,   true, false,  true, Dali::Material::Property::BLENDING_MODE )
DALI_PROPERTY( "blend-equation",                  STRING,   true, false,  true, Dali::Material::Property::BLEND_EQUATION )
DALI_PROPERTY( "source-blend-factor-rgb",         STRING,   true, false,  true, Dali::Material::Property::BLENDING_SRC_FACTOR_RGB )
DALI_PROPERTY( "destination-blend-factor-rgb",    STRING,   true, false,  true, Dali::Material::Property::BLENDING_DEST_FACTOR_RGB )
DALI_PROPERTY( "source-blend-factor-alpha",       STRING,   true, false,  true, Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA )
DALI_PROPERTY( "destination-blend-factor-alpha",  STRING,   true, false,  true, Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA )
DALI_PROPERTY( "blend-color",                     VECTOR4,  true, true,   true, Dali::Material::Property::BLEND_COLOR )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> MATERIAL_IMPL = { DEFAULT_PROPERTY_DETAILS };

} // unnamed namespace

MaterialPtr Material::New()
{
  return MaterialPtr( new Material() );
}

unsigned int Material::GetDefaultPropertyCount() const
{
  return MATERIAL_IMPL.GetDefaultPropertyCount();
}

void Material::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  MATERIAL_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Material::GetDefaultPropertyName(Property::Index index) const
{
  return MATERIAL_IMPL.GetDefaultPropertyName( index );
}

Property::Index Material::GetDefaultPropertyIndex( const std::string& name ) const
{
  return MATERIAL_IMPL.GetDefaultPropertyIndex( name );
}

bool Material::IsDefaultPropertyWritable( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyWritable( index );
}

bool Material::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Material::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Material::GetDefaultPropertyType( Property::Index index ) const
{
  return MATERIAL_IMPL.GetDefaultPropertyType( index );
}

void Material::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  MATERIAL_IMPL.SetDefaultProperty( index, propertyValue );
}

void Material::SetSceneGraphProperty( Property::Index index,
                                      const CustomProperty& entry,
                                      const Property::Value& value )
{
  MATERIAL_IMPL.SetSceneGraphProperty( index, entry, value );
}

Property::Value Material::GetDefaultProperty( Property::Index index ) const
{
  return MATERIAL_IMPL.GetDefaultProperty( index );
}

const SceneGraph::PropertyOwner* Material::GetPropertyOwner() const
{
  return MATERIAL_IMPL.GetPropertyOwner();
}

const SceneGraph::PropertyOwner* Material::GetSceneObject() const
{
  return MATERIAL_IMPL.GetSceneObject();
}

const SceneGraph::PropertyBase* Material::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return MATERIAL_IMPL.GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* Material::GetSceneObjectInputProperty( Property::Index index ) const
{
  return MATERIAL_IMPL.GetSceneObjectInputProperty( index );
}

int Material::GetPropertyComponentIndex( Property::Index index ) const
{
  return MATERIAL_IMPL.GetPropertyComponentIndex( index );
}

} // namespace Internal
} // namespace Dali

