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
#include <dali/internal/event/rendering/shader-impl.h> // Dali::Internal::Shader

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/shader-effects/shader-effect.h> // Dali::ShaderEffect::GeometryHints // TODO: MESH_REWORK REMOVE
#include <dali/devel-api/rendering/shader.h> // Dali::Shader
#include <dali/devel-api/scripting/scripting.h>

#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name             |type    |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "program",       MAP,     true,     false,     false,  Dali::Shader::Property::PROGRAM )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> SHADER_IMPL = { DEFAULT_PROPERTY_DETAILS };

Dali::Scripting::StringEnum ShaderHintsTable[] =
  { { "HINT_NONE",                     Dali::Shader::HINT_NONE},
    { "HINT_REQUIRES_SELF_DEPTH_TEST", Dali::Shader::HINT_REQUIRES_SELF_DEPTH_TEST},
    { "HINT_OUTPUT_IS_TRANSPARENT",    Dali::Shader::HINT_OUTPUT_IS_TRANSPARENT},
    { "HINT_OUTPUT_IS_OPAQUE",         Dali::Shader::HINT_OUTPUT_IS_OPAQUE},
    { "HINT_MODIFIES_GEOMETRY",        Dali::Shader::HINT_MODIFIES_GEOMETRY}
  };

const unsigned int ShaderHintsTableSize = sizeof( ShaderHintsTable ) / sizeof( ShaderHintsTable[0] );

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Shader ), typeid( Dali::Handle ), Create );

#define TOKEN_STRING(x) (#x)

void AppendString(std::string& to, const std::string& append)
{
  if(to.size())
  {
    to += ",";
  }
  to += append;
}

Property::Value HintString(const Dali::Shader::ShaderHints& hints)
{
  std::string s;

  if(hints == Dali::Shader::HINT_NONE)
  {
    s = "HINT_NONE";
  }

  if(hints & Dali::Shader::HINT_REQUIRES_SELF_DEPTH_TEST)
  {
    AppendString(s, "HINT_REQUIRES_SELF_DEPTH_TEST");
  }

  if(hints & Dali::Shader::HINT_OUTPUT_IS_TRANSPARENT)
  {
    AppendString(s, "HINT_OUTPUT_IS_TRANSPARENT");
  }

  if(hints & Dali::Shader::HINT_OUTPUT_IS_OPAQUE)
  {
    AppendString(s, "HINT_OUTPUT_IS_OPAQUE");
  }

  if(hints & Dali::Shader::HINT_MODIFIES_GEOMETRY)
  {
    AppendString(s, "HINT_MODIFIES_GEOMETRY");
  }

  return Property::Value(s);
}


} // unnamed namespace

ShaderPtr Shader::New( const std::string& vertexShader,
                       const std::string& fragmentShader,
                       Dali::Shader::ShaderHints hints )
{
  //TODO: MESH_REWORK
  ShaderPtr shader( new Shader() );
  shader->Initialize( vertexShader, fragmentShader, hints );
  return shader;
}

const SceneGraph::Shader* Shader::GetShaderSceneObject() const
{
  return mSceneObject;
}

SceneGraph::Shader* Shader::GetShaderSceneObject()
{
  return mSceneObject;
}

unsigned int Shader::GetDefaultPropertyCount() const
{
  return SHADER_IMPL.GetDefaultPropertyCount();
}

void Shader::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  SHADER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Shader::GetDefaultPropertyName(Property::Index index) const
{
  return SHADER_IMPL.GetDefaultPropertyName( index );
}

Property::Index Shader::GetDefaultPropertyIndex( const std::string& name ) const
{
  return SHADER_IMPL.GetDefaultPropertyIndex( name );
}

bool Shader::IsDefaultPropertyWritable( Property::Index index ) const
{
  return SHADER_IMPL.IsDefaultPropertyWritable( index );
}

bool Shader::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return SHADER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Shader::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return SHADER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Shader::GetDefaultPropertyType( Property::Index index ) const
{
  return SHADER_IMPL.GetDefaultPropertyType( index );
}

void Shader::SetDefaultProperty( Property::Index index,
                                 const Property::Value& propertyValue )
{
  switch(index)
  {
    case Dali::Shader::Property::PROGRAM:
    {
      if( propertyValue.GetType() == Property::MAP )
      {
        Dali::Property::Map* map = propertyValue.GetMap();
        std::string vertex;
        std::string fragment;
        Dali::Shader::ShaderHints hints(Dali::Shader::HINT_NONE);

        if( Property::Value* value = map->Find("vertex") )
        {
          vertex = value->Get<std::string>();
        }

        if( Property::Value* value = map->Find("fragment") )
        {
          fragment = value->Get<std::string>();
        }

        if( Property::Value* value = map->Find("hints") )
        {
          static_cast<void>( // ignore return
            Scripting::GetEnumeration< Dali::Shader::ShaderHints >(value->Get<std::string>().c_str(),
                                                                   ShaderHintsTable, ShaderHintsTableSize, hints)
            );
        }

        Initialize(vertex, fragment, hints );
      }
      else
      {
        DALI_LOG_WARNING( "Shader program property should be a map\n" );
      }
      break;
    }
  }
}

void Shader::SetSceneGraphProperty( Property::Index index,
                                    const PropertyMetadata& entry,
                                    const Property::Value& value )
{
  SHADER_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  OnPropertySet(index, value);
}

Property::Value Shader::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch(index)
  {
    case Dali::Shader::Property::PROGRAM:
    {
      Dali::Property::Map map;
      if( mShaderData )
      {
        map["vertex"] = Property::Value(mShaderData->GetVertexShader());
        map["fragment"] = Property::Value(mShaderData->GetFragmentShader());
        map["hints"] = HintString(mShaderData->GetHints());
      }
      value = map;
      break;
    }
  }

  return value;
}

const SceneGraph::PropertyOwner* Shader::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Shader::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Shader::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable( index ) && "Property is not animatable" );
  const SceneGraph::PropertyBase* property = NULL;

  property = SHADER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                          &Shader::FindAnimatableProperty,
                                                          &Shader::FindCustomProperty,
                                                          index );

  if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    DALI_ASSERT_ALWAYS( 0 && "Property is not animatable" );
  }

  return property;
}

const PropertyInputImpl* Shader::GetSceneObjectInputProperty( Property::Index index ) const
{
  PropertyMetadata* property = NULL;

  if(index >= PROPERTY_CUSTOM_START_INDEX )
  {
    property = FindCustomProperty( index );
  }
  else
  {
    property = FindAnimatableProperty( index );
  }

  DALI_ASSERT_ALWAYS( property && "property index is invalid" );
  return property->GetSceneGraphProperty();
}

int Shader::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

Shader::Shader()
  : mSceneObject( NULL ),
    mShaderData( NULL )
{
}

void Shader::Initialize(
  const std::string& vertexSource,
  const std::string& fragmentSource,
  Dali::Shader::ShaderHints hints )
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  // @todo MESH_REWORK - Pass hints directly to a new scene graph shader
  int effectHint = Dali::ShaderEffect::HINT_NONE;
  if( hints & Dali::Shader::HINT_OUTPUT_IS_TRANSPARENT )
  {
    effectHint |= Dali::ShaderEffect::HINT_BLENDING;
  }

  if( hints & Dali::Shader::HINT_REQUIRES_SELF_DEPTH_TEST )
  {
    effectHint |= Dali::ShaderEffect::HINT_DEPTH_BUFFER;
  }

  if( (hints & Dali::Shader::HINT_MODIFIES_GEOMETRY) == 0x0 )
  {
    effectHint |= Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY;
  }
  Dali::ShaderEffect::GeometryHints shaderEffectHint = static_cast<Dali::ShaderEffect::GeometryHints>( effectHint );

  mSceneObject = new SceneGraph::Shader( shaderEffectHint );

  // Add to update manager
  AddShaderMessage( updateManager, *mSceneObject );

  // Try to load a precompiled shader binary for the source pair:
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ShaderFactory& shaderFactory = tls.GetShaderFactory();
  size_t shaderHash;
  mShaderData = shaderFactory.Load( vertexSource, fragmentSource, hints, shaderHash );

  // Add shader program to scene-object using a message to the UpdateManager
  SetShaderProgramMessage( updateManager, *mSceneObject, mShaderData, (hints & Dali::Shader::HINT_MODIFIES_GEOMETRY) != 0x0 );
  eventThreadServices.RegisterObject( this );
}

Shader::~Shader()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveShaderMessage( updateManager, *mSceneObject);

    eventThreadServices.UnregisterObject( this );
  }
}


} // namespace Internal
} // namespace Dali
