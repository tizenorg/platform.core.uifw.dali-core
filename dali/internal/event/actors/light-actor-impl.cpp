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
#include <dali/internal/event/common/property-index-ranges.h>

namespace Dali
{

const Property::Index LightActor::LIGHT_TYPE              = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT;
const Property::Index LightActor::ENABLE                  = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 1;
const Property::Index LightActor::FALL_OFF                = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 2;
const Property::Index LightActor::SPOT_ANGLE              = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 3;
const Property::Index LightActor::AMBIENT_COLOR           = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 4;
const Property::Index LightActor::DIFFUSE_COLOR           = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 5;
const Property::Index LightActor::SPECULAR_COLOR          = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 6;
const Property::Index LightActor::DIRECTION               = Internal::DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 7;

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Dali::LightActor::New();
}

TypeRegistration mType( typeid(Dali::LightActor), typeid(Dali::Actor), Create );

const Internal::PropertyDetails DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[] =
{
  // Name             Type              writable animatable constraint-input
  { "light-type",     Property::STRING,   true,    false,   true },  // LIGHT_TYPE
  { "enable",         Property::BOOLEAN,  true,    false,   true },  // ENABLE
  { "fall-off",       Property::VECTOR2,  true,    false,   true },  // FALL_OFF
  { "spot-angle",     Property::VECTOR2,  true,    false,   true },  // SPOT_ANGLE
  { "ambient-color",  Property::VECTOR3,  true,    false,   true },  // AMBIENT_COLOR
  { "diffuse-color",  Property::VECTOR3,  true,    false,   true },  // DIFFUSE_COLOR
  { "specular-color", Property::VECTOR3,  true,    false,   true },  // SPECULAR_COLOR
  { "direction",      Property::VECTOR3,  true,    false,   true },  // DIRECTION
};
const int DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT = sizeof( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS ) / sizeof( Internal::PropertyDetails );

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
  return Actor::GetDefaultPropertyCount() + DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT;
}

void LightActor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  Actor::GetDefaultPropertyIndices( indices ); // Actor class properties

  indices.reserve( indices.size() + DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT );

  int index = DEFAULT_ACTOR_PROPERTY_MAX_COUNT;
  for ( int i = 0; i < DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT; ++i, ++index )
  {
    indices.push_back( index );
  }
}

bool LightActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::IsDefaultPropertyWritable(index) ;
  }
  else
  {
    return true;
  }
}

bool LightActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::IsDefaultPropertyAnimatable(index) ;
  }
  else
  {
    return false;
  }
}

bool LightActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAConstraintInput(index);
  }
  return true; // Our properties can be used as input to constraints.
}

Property::Type LightActor::GetDefaultPropertyType( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyType(index) ;
  }
  else
  {
    index -= DEFAULT_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[index].type;
    }
    else
    {
      // index out-of-bounds
      return Property::NONE;
    }
  }
}

const char* LightActor::GetDefaultPropertyName( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyName(index) ;
  }
  else
  {
    index -= DEFAULT_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[index].name;
    }
    else
    {
      return NULL;
    }
  }
}

Property::Index LightActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in current class' default properties
  for( unsigned int i = 0; i < DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
    {
      index = i;
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
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    Actor::SetDefaultProperty(index, propertyValue) ;
  }
  else
  {
    switch(index)
    {
      case Dali::LightActor::LIGHT_TYPE:
      {
        mLightAttachment->SetType(LightTypeEnum(propertyValue.Get<std::string>()));
        break;
      }
      case Dali::LightActor::ENABLE:
      {
        SetActive(propertyValue.Get<bool>());
        break;
      }
      case Dali::LightActor::FALL_OFF:
      {
        mLightAttachment->SetFallOff(propertyValue.Get<Vector2>());
        break;
      }
      case Dali::LightActor::SPOT_ANGLE:
      {
        mLightAttachment->SetSpotAngle(propertyValue.Get<Vector2>());
        break;
      }
      case Dali::LightActor::AMBIENT_COLOR:
      {
        mLightAttachment->SetAmbientColor(propertyValue.Get<Vector3>());
        break;
      }
      case Dali::LightActor::DIFFUSE_COLOR:
      {
        mLightAttachment->SetDiffuseColor(propertyValue.Get<Vector3>());
        break;
      }
      case Dali::LightActor::SPECULAR_COLOR:
      {
        mLightAttachment->SetSpecularColor(propertyValue.Get<Vector3>());
        break;
      }
      case Dali::LightActor::DIRECTION:
      {
        mLightAttachment->SetDirection(propertyValue.Get<Vector3>());
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value LightActor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret ;
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = Actor::GetDefaultProperty(index) ;
  }
  else
  {
    switch(index)
    {
      case Dali::LightActor::LIGHT_TYPE:
      {
        ret = LightTypeString(mLightAttachment->GetType());
        break;
      }
      case Dali::LightActor::ENABLE:
      {
        ret = mIsActive;
        break;
      }
      case Dali::LightActor::FALL_OFF:
      {
        ret = mLightAttachment->GetFallOff();
        break;
      }
      case Dali::LightActor::SPOT_ANGLE:
      {
        ret = mLightAttachment->GetSpotAngle();
        break;
      }
      case Dali::LightActor::AMBIENT_COLOR:
      {
        ret = mLightAttachment->GetAmbientColor();
        break;
      }
      case Dali::LightActor::DIFFUSE_COLOR:
      {
        ret = mLightAttachment->GetDiffuseColor();
        break;
      }
      case Dali::LightActor::SPECULAR_COLOR:
      {
        ret = mLightAttachment->GetSpecularColor();
        break;
      }
      case Dali::LightActor::DIRECTION:
      {
        ret = mLightAttachment->GetDirection();
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)
  }

  return ret ;
}


} // namespace Internal

} // namespace Dali
