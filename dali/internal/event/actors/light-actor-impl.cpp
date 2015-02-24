/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/light-actor-impl.h>

// INTERNAL HEADER
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/property-helper.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

//              Name             Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "light-type",     STRING,   true,    false,   true,   Dali::LightActor::Property::LightType     )
DALI_PROPERTY( "enable",         BOOLEAN,  true,    false,   true,   Dali::LightActor::Property::Enable        )
DALI_PROPERTY( "fall-off",       VECTOR2,  true,    false,   true,   Dali::LightActor::Property::FallOff       )
DALI_PROPERTY( "spot-angle",     VECTOR2,  true,    false,   true,   Dali::LightActor::Property::SpotAngle     )
DALI_PROPERTY( "ambient-color",  VECTOR3,  true,    false,   true,   Dali::LightActor::Property::AmbientColor  )
DALI_PROPERTY( "diffuse-color",  VECTOR3,  true,    false,   true,   Dali::LightActor::Property::DiffuseColor  )
DALI_PROPERTY( "specular-color", VECTOR3,  true,    false,   true,   Dali::LightActor::Property::SpecularColor )
DALI_PROPERTY( "direction",      VECTOR3,  true,    false,   true,   Dali::LightActor::Property::Direction     )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX )

BaseHandle Create()
{
  return Dali::LightActor::New();
}

TypeRegistration mType( typeid( Dali::LightActor ), typeid( Dali::Actor ), Create );

LightType LightTypeEnum(const std::string &stringValue)
{
  LightType ret(AMBIENT);

  if(stringValue == "AMBIENT")
  {
    ret =  AMBIENT;
  }
  else if(stringValue == "DIRECTIONAL")
  {
    ret =  DIRECTIONAL;
  }
  else if(stringValue == "SPOT")
  {
    ret =  SPOT;
  }
  else if(stringValue == "POINT")
  {
    ret =  POINT;
  }
  else
  {
    DALI_LOG_WARNING("Unknown Light Type:%s\n", stringValue.c_str());
  }

  return ret;
}

std::string LightTypeString(const LightType type)
{
  std::string ret("AMBIENT");

  switch ( type )
  {
    case AMBIENT:
    {
      ret = "AMBIENT";
      break;
    }

    case DIRECTIONAL:
    {
      ret = "DIRECTIONAL";
      break;
    }

    case SPOT:
    {
      ret = "SPOT";
      break;
    }
    case POINT:
    {
      ret = "POINT";
      break;
    }
  }
  return ret;
}

} // namespace


LightActorPtr LightActor::New()
{
  LightActorPtr actor(new LightActor());

  // Second-phase construction

  actor->Initialize();

  // Create the attachment
  actor->mLightAttachment = LightAttachment::New( *actor->mNode );
  actor->Attach(*actor->mLightAttachment);
  actor->mLightAttachment->SetName(actor->GetName());

  return actor;
}


void LightActor::OnInitialize()
{
}

LightActor::LightActor()
: Actor( Actor::BASIC ),
  mIsActive(false)
{
}

LightActor::~LightActor()
{
}

void LightActor::SetLight(Dali::Light light)
{
  Internal::LightPtr lightPtr(&GetImplementation(light));
  mLightAttachment->SetLight(lightPtr);
  mLightAttachment->SetActive(true);
}

Dali::Light LightActor::GetLight() const
{
  Internal::LightPtr lightPtr(mLightAttachment->GetLight());
  return Dali::Light(lightPtr.Get());
}

void LightActor::SetActive(bool active)
{
  mLightAttachment->SetActive(active);
  mIsActive = active;
}

bool LightActor::GetActive()
{
  return mIsActive;
}

unsigned int LightActor::GetDefaultPropertyCount() const
{
  return Actor::GetDefaultPropertyCount() + DEFAULT_PROPERTY_COUNT;
}

void LightActor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  Actor::GetDefaultPropertyIndices( indices ); // Actor class properties

  indices.reserve( indices.size() + DEFAULT_PROPERTY_COUNT );

  int index = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i, ++index )
  {
    indices.push_back( index );
  }
}

bool LightActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyWritable( index ) ;
  }

  return DEFAULT_PROPERTY_DETAILS[ index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX ].writable;
}

bool LightActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAnimatable( index ) ;
  }

  return DEFAULT_PROPERTY_DETAILS[ index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX ].animatable;
}

bool LightActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAConstraintInput( index );
  }
  return DEFAULT_PROPERTY_DETAILS[ index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX ].constraintInput;
}

Property::Type LightActor::GetDefaultPropertyType( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::GetDefaultPropertyType( index ) ;
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out-of-bounds
  return Property::NONE;
}

const char* LightActor::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::GetDefaultPropertyName( index ) ;
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  return NULL;
}

Property::Index LightActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in current class' default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
    {
      index = i + DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
      break;
    }
  }

  // If not found, check in base class
  if( Property::INVALID_INDEX == index )
  {
    index = Actor::GetDefaultPropertyIndex( name );
  }

  return index;
}

void LightActor::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    Actor::SetDefaultProperty( index, propertyValue ) ;
  }
  else
  {
    switch(index)
    {
      case Dali::LightActor::Property::LightType:
      {
        mLightAttachment->SetType( LightTypeEnum(propertyValue.Get<std::string>() ) );
        break;
      }
      case Dali::LightActor::Property::Enable:
      {
        SetActive( propertyValue.Get<bool>() );
        break;
      }
      case Dali::LightActor::Property::FallOff:
      {
        mLightAttachment->SetFallOff( propertyValue.Get<Vector2>() );
        break;
      }
      case Dali::LightActor::Property::SpotAngle:
      {
        mLightAttachment->SetSpotAngle( propertyValue.Get<Vector2>() );
        break;
      }
      case Dali::LightActor::Property::AmbientColor:
      {
        mLightAttachment->SetAmbientColor( propertyValue.Get<Vector3>() );
        break;
      }
      case Dali::LightActor::Property::DiffuseColor:
      {
        mLightAttachment->SetDiffuseColor( propertyValue.Get<Vector3>() );
        break;
      }
      case Dali::LightActor::Property::SpecularColor:
      {
        mLightAttachment->SetSpecularColor( propertyValue.Get<Vector3>() );
        break;
      }
      case Dali::LightActor::Property::Direction:
      {
        mLightAttachment->SetDirection( propertyValue.Get<Vector3>() );
        break;
      }
      default:
      {
        DALI_LOG_WARNING( "Unknown property (%d)\n", index );
        break;
      }
    } // switch(index)

  } // else
}

Property::Value LightActor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret ;
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    ret = Actor::GetDefaultProperty( index ) ;
  }
  else
  {
    switch( index )
    {
      case Dali::LightActor::Property::LightType:
      {
        ret = LightTypeString( mLightAttachment->GetType() );
        break;
      }
      case Dali::LightActor::Property::Enable:
      {
        ret = mIsActive;
        break;
      }
      case Dali::LightActor::Property::FallOff:
      {
        ret = mLightAttachment->GetFallOff();
        break;
      }
      case Dali::LightActor::Property::SpotAngle:
      {
        ret = mLightAttachment->GetSpotAngle();
        break;
      }
      case Dali::LightActor::Property::AmbientColor:
      {
        ret = mLightAttachment->GetAmbientColor();
        break;
      }
      case Dali::LightActor::Property::DiffuseColor:
      {
        ret = mLightAttachment->GetDiffuseColor();
        break;
      }
      case Dali::LightActor::Property::SpecularColor:
      {
        ret = mLightAttachment->GetSpecularColor();
        break;
      }
      case Dali::LightActor::Property::Direction:
      {
        ret = mLightAttachment->GetDirection();
        break;
      }
      default:
      {
        DALI_LOG_WARNING( "Unknown property (%d)\n", index );
        break;
      }
    } // switch(index)
  }

  return ret ;
}


} // namespace Internal

} // namespace Dali
