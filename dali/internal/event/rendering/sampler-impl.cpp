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
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/rendering/sampler.h> // Dali::Internal::Sampler
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/rendering/scene-graph-sampler.h> // Dali::Internal::SceneGraph::Sampler
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/render/renderers/render.sampler.h>

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
DALI_PROPERTY( "minification-filter",   INTEGER,   true, false,  true, Dali::Sampler::Property::MINIFICATION_FILTER )
DALI_PROPERTY( "magnification-filter",  INTEGER,   true, false,  true, Dali::Sampler::Property::MAGNIGICATION_FILTER )
DALI_PROPERTY( "u-wrap",                INTEGER,   true, false,  true, Dali::Sampler::Property::U_WRAP )
DALI_PROPERTY( "v-wrap",                INTEGER,   true, false,  true, Dali::Sampler::Property::V_WRAP )
DALI_PROPERTY( "affects-transparency",  BOOLEAN,  true, false,  true, Dali::Sampler::Property::AFFECTS_TRANSPARENCY )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> SAMPLER_IMPL = { DEFAULT_PROPERTY_DETAILS };

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Sampler ), typeid( Dali::Handle ), Create );

} // unnamed namespace

SamplerPtr Sampler::New( )
{
  SamplerPtr sampler( new Sampler() );
  sampler->Initialize();
  return sampler;
}

//
//void Sampler::SetImage( ImagePtr& image )
//{
//  // Keep a reference to the image object
//  mImageConnector.Set( image, OnStage() );
//
//  // sceneObject is being used in a separate thread; queue a message to set
//  if( mOnStage )
//  {
//    unsigned int resourceId = image->GetResourceId();
//    if( resourceId != 0 )
//    {
//      //FERRAN: TODO
//     // SetTextureMessage( GetEventThreadServices(), *mSceneObject, resourceId );
//    }
//  }
//}
//
//ImagePtr Sampler::GetImage() const
//{
//  return mImageConnector.Get();
//}

void Sampler::SetFilterMode( Dali::Sampler::FilterMode minFilter, Dali::Sampler::FilterMode magFilter )
{
  if( NULL != mSceneObject )
  {
    //FERRAN: TODO
    //SetFilterModeMessage( GetEventThreadServices(), *mSceneObject, minFilter, magFilter );
  }
}

void Sampler::SetWrapMode( Dali::Sampler::WrapMode uWrap, Dali::Sampler::WrapMode vWrap )
{
  if( NULL != mSceneObject )
  {
    //FERRAN: TODO
    //SetWrapModeMessage( GetEventThreadServices(), *mSceneObject, uWrap, vWrap );
  }
}
//
//void Sampler::SetAffectsTransparency( bool affectsTransparency )
//{
//  if( NULL != mSceneObject )
//  {
//    //SceneGraph::DoubleBufferedPropertyMessage<bool>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mAffectsTransparency, &SceneGraph::DoubleBufferedProperty<bool>::Set, affectsTransparency );
//    //SceneGraph::DoubleBufferedPropertyMessage<bool>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mAffectsTransparency, &SceneGraph::DoubleBufferedProperty<bool>::Set, affectsTransparency );
//  }
//}

//const SceneGraph::Sampler* Sampler::GetSamplerSceneObject() const
//{
//  return mSceneObject;
//}
//
//SceneGraph::Sampler* Sampler::GetSamplerSceneObject()
//{
//  return mSceneObject;
//}

unsigned int Sampler::GetDefaultPropertyCount() const
{
  return SAMPLER_IMPL.GetDefaultPropertyCount();
}

void Sampler::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  SAMPLER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Sampler::GetDefaultPropertyName(Property::Index index) const
{
  return SAMPLER_IMPL.GetDefaultPropertyName( index );
}

Property::Index Sampler::GetDefaultPropertyIndex( const std::string& name ) const
{
  return SAMPLER_IMPL.GetDefaultPropertyIndex( name );
}

bool Sampler::IsDefaultPropertyWritable( Property::Index index ) const
{
  return SAMPLER_IMPL.IsDefaultPropertyWritable( index );
}

bool Sampler::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return SAMPLER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Sampler::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return SAMPLER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Sampler::GetDefaultPropertyType( Property::Index index ) const
{
  return SAMPLER_IMPL.GetDefaultPropertyType( index );
}

void Sampler::SetDefaultProperty( Property::Index index,
                                  const Property::Value& propertyValue )
{
//  switch( index )
//  {
//    case Dali::Sampler::Property::MINIFICATION_FILTER:
//    {
//      SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mMinFilter, &SceneGraph::DoubleBufferedProperty<int>::Set, propertyValue.Get<int>() );
//      break;
//    }
//    case Dali::Sampler::Property::MAGNIGICATION_FILTER:
//    {
//      SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mMagFilter, &SceneGraph::DoubleBufferedProperty<int>::Set, propertyValue.Get<int>() );
//      break;
//    }
//    case Dali::Sampler::Property::U_WRAP:
//    {
//      SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mUWrapMode, &SceneGraph::DoubleBufferedProperty<int>::Set, propertyValue.Get<int>() );
//      break;
//    }
//    case Dali::Sampler::Property::V_WRAP:
//    {
//      SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mVWrapMode, &SceneGraph::DoubleBufferedProperty<int>::Set, propertyValue.Get<int>() );
//      break;
//    }
//    case Dali::Sampler::Property::AFFECTS_TRANSPARENCY:
//    {
//      SceneGraph::DoubleBufferedPropertyMessage<bool>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mAffectsTransparency, &SceneGraph::DoubleBufferedProperty<bool>::Set, propertyValue.Get<bool>() );
//      break;
//    }
//  }
}

void Sampler::SetSceneGraphProperty( Property::Index index,
                                     const PropertyMetadata& entry,
                                     const Property::Value& value )
{
  SAMPLER_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  OnPropertySet(index, value);
}

Property::Value Sampler::GetDefaultProperty( Property::Index index ) const
{
  //BufferIndex bufferIndex = GetEventThreadServices().GetEventBufferIndex();
  Property::Value value;

  switch( index )
  {
    case Dali::Sampler::Property::MINIFICATION_FILTER:
    {
      value = mSceneObject->mMinFilter;
      break;
    }
    case Dali::Sampler::Property::MAGNIGICATION_FILTER:
    {
      value = mSceneObject->mMagFilter;
      break;
    }
    case Dali::Sampler::Property::U_WRAP:
    {
      value = mSceneObject->mUWrapMode;
      break;
    }
    case Dali::Sampler::Property::V_WRAP:
    {
      value = mSceneObject->mVWrapMode;
      break;
    }
    case Dali::Sampler::Property::AFFECTS_TRANSPARENCY:
    {
      value = true;//mSceneObject->mAffectsTransparency;
      break;
    }
  }
  return value;
}

const SceneGraph::PropertyOwner* Sampler::GetPropertyOwner() const
{
  return 0;
}

const SceneGraph::PropertyOwner* Sampler::GetSceneObject() const
{
  return 0;
}

const SceneGraph::PropertyBase* Sampler::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable( index ) && "Property is not animatable" );

  const SceneGraph::PropertyBase* property = NULL;

  if( OnStage() )
  {
    property = SAMPLER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                             &Sampler::FindAnimatableProperty,
                                                             &Sampler::FindCustomProperty,
                                                             index );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      // No animatable default props
      DALI_ASSERT_ALWAYS( 0 && "Property is not animatable" );
    }
  }

  return property;
}

const PropertyInputImpl* Sampler::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;
//
//  if( OnStage() )
//  {
//    const SceneGraph::PropertyBase* baseProperty =
//      SAMPLER_IMPL.GetRegisteredSceneGraphProperty( this,
//                                                    &Sampler::FindAnimatableProperty,
//                                                    &Sampler::FindCustomProperty,
//                                                    index );
//    property = static_cast<const PropertyInputImpl*>( baseProperty );
//
//    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
//    {
//      switch( index )
//      {
//        case Dali::Sampler::Property::MINIFICATION_FILTER:
//        {
//          property = &mSceneObject->mMinFilter;
//          break;
//        }
//        case Dali::Sampler::Property::MAGNIGICATION_FILTER:
//        {
//          property = &mSceneObject->mMagFilter;
//          break;
//        }
//        case Dali::Sampler::Property::U_WRAP:
//        {
//          property = &mSceneObject->mUWrapMode;
//          break;
//        }
//        case Dali::Sampler::Property::V_WRAP:
//        {
//          property = &mSceneObject->mVWrapMode;
//          break;
//        }
//        case Dali::Sampler::Property::AFFECTS_TRANSPARENCY:
//        {
//          property = &mSceneObject->mAffectsTransparency;
//          break;
//        }
//      }
//    }
//  }

  return property;
}

int Sampler::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool Sampler::OnStage() const
{
  return mOnStage;
}

void Sampler::Connect()
{
  mOnStage = true;

  mImageConnector.OnStageConnect();

  // sceneObject is being used in a separate thread; queue a message to set
  //unsigned int resourceId = mImageConnector.Get()->GetResourceId();
  //SetTextureMessage( GetEventThreadServices(), *mSceneObject, resourceId );
}

void Sampler::Disconnect()
{
  mOnStage = false;

  mImageConnector.OnStageDisconnect();
}

Sampler::Sampler()
: mSceneObject( NULL ),
  mOnStage( false )
{
}

void Sampler::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = new Render::Sampler();
  AddSamplerMessage( updateManager, *mSceneObject );

  eventThreadServices.RegisterObject( this );
}

Sampler::~Sampler()
{
  //FERRAN: TODO
  if( EventThreadServices::IsCoreRunning() )
  {
    //EventThreadServices& eventThreadServices = GetEventThreadServices();
    //SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    //RemoveSamplerMessage( updateManager, *mSceneObject );

    //eventThreadServices.UnregisterObject( this );
  }
}

} // namespace Internal
} // namespace Dali
