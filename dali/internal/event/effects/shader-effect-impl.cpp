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
#include <dali/internal/event/effects/shader-effect-impl.h>

// EXTERNAL INCLUDES
#include <fstream>

// INTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/scripting/scripting.h>
#include "dali-shaders.h"
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::UniformMeta;
using Dali::Internal::SceneGraph::Shader;
using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;
using Dali::Internal::SceneGraph::PropertyBase;
using Dali::Internal::SceneGraph::RenderQueue;
using std::string;

namespace Dali
{

const Property::Index ShaderEffect::GRID_DENSITY        = 0;
const Property::Index ShaderEffect::IMAGE               = 1;
const Property::Index ShaderEffect::PROGRAM             = 2;
const Property::Index ShaderEffect::GEOMETRY_HINTS      = 3;

namespace Internal
{

ShaderEffect::DefaultPropertyLookup* ShaderEffect::mDefaultPropertyLookup = NULL;

namespace
{

struct WrapperStrings
{
  string vertexShaderPrefix;
  string fragmentShaderPrefix;
  string vertexShaderPostfix;
  string fragmentShaderPostfix;
};

WrapperStrings customShaderWrappers [] =
{
  {
    CustomImagePrefixVertex, CustomImagePrefixFragment,
    CustomImagePostfixVertex, CustomImagePostfixFragment
  },
  {
    CustomFontPrefixVertex, CustomFontPrefixFragment,
    CustomFontPostfixVertex, CustomFontPostfixFragment
  },
  {
    CustomMeshPrefixVertex, CustomMeshPrefixFragment,
    MeshVertex, MeshFragment
  }
};

BaseHandle Create()
{
  Internal::ShaderEffectPtr internal = Internal::ShaderEffect::New( );

  ShaderFactory::LoadTextSubtypeShaders(internal);

  return Dali::ShaderEffect(internal.Get());
}

TypeRegistration mType( typeid(Dali::ShaderEffect), typeid(Dali::Handle), Create );

const string DEFAULT_PROPERTY_NAMES[] =
{
  "grid-density",
  "image",
  "program",
  "geometry-hints",
};
const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_NAMES ) / sizeof( string );

const Property::Type DEFAULT_PROPERTY_TYPES[DEFAULT_PROPERTY_COUNT] =
{
  Property::FLOAT,    // "grid-density",
  Property::MAP,      // "image",
  Property::MAP,      // "program",
  Property::INTEGER,  // "geometry-hints",
};

string GetFileContents( const string& filename )
{
  std::ifstream input( filename.c_str() );
  return string( std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() );
}

string GetShader(const string& field, const Property::Value& property)
{
  if( property.HasKey(field) )
  {
    DALI_ASSERT_ALWAYS(property.GetValue(field).GetType() == Property::STRING && "Shader property is not a string" );

    // we could also check here for an array of strings as convenience for json not having multi line strings
    return property.GetValue(field).Get<string>();
  }
  else
  {
    // convention of shader field appended with -filename signifies a file
    string filenameKey(string(field) + string("-filename"));

    if( property.HasKey( filenameKey ) )
    {
      DALI_ASSERT_ALWAYS(property.GetValue(filenameKey).GetType() == Property::STRING && "Shader filename property is not a string" );
      // this should eventually be done by an adaptor interface
      return GetFileContents( property.GetValue(filenameKey).Get<string>() );
    }
  }

  // if we got here
  return string();
}

} // anon namespace


ShaderEffectPtr ShaderEffect::New( Dali::ShaderEffect::GeometryHints hints )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  UpdateManager& updateManager = tls.GetUpdateManager();
  ShaderFactory& shaderFactory = tls.GetShaderFactory();

  // Create a new scene-object, temporarily owned
  Shader* sceneObject = new Shader( hints );
  DALI_ASSERT_DEBUG( NULL != sceneObject );

  ShaderEffectPtr shaderEffect( new ShaderEffect( updateManager, shaderFactory, *sceneObject ) );
  shaderEffect->RegisterObject();

  return shaderEffect;
}

ShaderEffectPtr ShaderEffect::New( const string& vertexShader,
                                   const string& fragmentShader,
                                   GeometryType geometryType,
                                   Dali::ShaderEffect::GeometryHints hints )
{
  return NewWithPrefix( "", vertexShader, "", fragmentShader, geometryType, hints);
}

ShaderEffectPtr ShaderEffect::NewWithPrefix( const string& vertexShaderPrefix,
                                             const string& vertexShader,
                                             const string& fragmentShaderPrefix,
                                             const string& fragmentShader,
                                             GeometryType geometryTypes,
                                             Dali::ShaderEffect::GeometryHints hints )
{
  ShaderEffectPtr shaderEffect( New(hints) );
  ShaderFactory::LoadTextSubtypeShaders(shaderEffect);

  shaderEffect->SetPrograms( geometryTypes, vertexShaderPrefix, vertexShader, fragmentShaderPrefix, fragmentShader );
  return shaderEffect;
}

ShaderEffectPtr ShaderEffect::New( const string& imageVertexShader,
                                   const string& imageFragmentShader,
                                   const string& textVertexShader,
                                   const string& textFragmentShader,
                                   const string& texturedMeshVertexShader,
                                   const string& texturedMeshFragmentShader,
                                   const string& meshVertexShader,
                                   const string& meshFragmentShader,
                                   Dali::ShaderEffect::GeometryHints hints )
{
  ShaderEffectPtr shaderEffect( New(hints) );

  ShaderFactory::LoadTextSubtypeShaders(shaderEffect);

  shaderEffect->SetWrappedProgram( GEOMETRY_TYPE_IMAGE, SHADER_SUBTYPE_ALL, "", "", imageVertexShader, imageFragmentShader );
  shaderEffect->SetWrappedProgram( GEOMETRY_TYPE_TEXT, SHADER_DEFAULT, "", "", textVertexShader, textFragmentShader );
  shaderEffect->SetWrappedProgram( GEOMETRY_TYPE_TEXTURED_MESH, SHADER_SUBTYPE_ALL, "", "", texturedMeshVertexShader, texturedMeshFragmentShader );
  shaderEffect->SetWrappedProgram( GEOMETRY_TYPE_MESH, SHADER_SUBTYPE_ALL, "", "", meshVertexShader, meshFragmentShader );

  return shaderEffect;
}

ShaderEffect::~ShaderEffect()
{
  DALI_ASSERT_DEBUG( mSceneObject != NULL );

  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    // Remove scene-object using a message to the UpdateManager
    RemoveShaderMessage( mUpdateManager, *mSceneObject );

    UnregisterObject();
  }
}

ShaderEffect::ShaderEffect( UpdateManager& updateManager, ShaderFactory& shaderFactory, Shader& sceneObject )
: mUpdateManager( updateManager ),
  mShaderFactory( shaderFactory ),
  mSceneObject( &sceneObject ),
  mConnectionCount (0),
  mGeometryHints( sceneObject.GetGeometryHints() )
{
  // Transfer shader ownership to a scene message
  AddShaderMessage( mUpdateManager, *mSceneObject );
}

void ShaderEffect::SetEffectImage( Dali::Image image )
{
  // if images are the same, do nothing
  if (mImage == image)
  {
    return;
  }

  if (mImage && mConnectionCount > 0)
  {
    // unset previous image
    GetImplementation(mImage).Disconnect();
  }

  // in case image is empty this will reset our image handle
  mImage = image;

  if (!image)
  {
    // mSceneShader can be in a separate thread; queue a setter message
    SetTextureIdMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, 0 );
  }
  else
  {
    // tell image that we're using it
    if (mConnectionCount > 0)
    {
      GetImplementation(mImage).Connect();
    }
    // mSceneShader can be in a separate thread; queue a setter message
    SetTextureIdMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, GetImplementation(mImage).GetResourceId() );
  }
}

void ShaderEffect::SetUniform( const string& name, Property::Value value, UniformCoordinateType uniformCoordinateType )
{
  // Register the property if it does not exist
  Property::Index index = GetPropertyIndex( name );
  if ( Property::INVALID_INDEX == index )
  {
    index = RegisterProperty( name, value );
  }

  SetProperty( index, value );

  SetCoordinateTypeMessage( mUpdateManager.GetEventToUpdate(), *mCustomMetadata[index], uniformCoordinateType );
}

void ShaderEffect::AttachExtension( Dali::ShaderEffect::Extension *extension )
{
  DALI_ASSERT_ALWAYS( extension != NULL && "Attaching uninitialized extension" );
  mExtension = IntrusivePtr<Dali::ShaderEffect::Extension>( extension );
}

Dali::ShaderEffect::Extension& ShaderEffect::GetExtension()
{
  DALI_ASSERT_ALWAYS( mExtension && "Getting uninitialized extension" );
  return *mExtension;
}

const Dali::ShaderEffect::Extension& ShaderEffect::GetExtension() const
{
  DALI_ASSERT_ALWAYS( mExtension && "Getting uninitialized extension" );
  return *mExtension;
}

void ShaderEffect::SetProgram( GeometryType geometryType, ShaderSubTypes subType,
                               const string& vertexSource, const string& fragmentSource,
                               ShaderEffect::FixedVertexShader fixedShader )
{
  SetProgramImpl(geometryType, subType, vertexSource, fragmentSource, fixedShader);
}

void ShaderEffect::SetProgram( GeometryType geometryType, ShaderSubTypes subType,
                               const string& vertexPrefix, const string& fragmentPrefix,
                               const string& vertexSource, const string& fragmentSource,
                               ShaderEffect::FixedVertexShader fixedShader )
{
  const string vertex( vertexPrefix + vertexSource );
  const string fragment( fragmentPrefix + fragmentSource );
  SetProgramImpl( geometryType, subType, vertex, fragment, fixedShader );
}

void ShaderEffect::Connect()
{
  ++mConnectionCount;

  if (mImage && mConnectionCount == 1)
  {
    GetImplementation(mImage).Connect();

    // Image may have changed resource due to load/release policy. Ensure correct texture ID is set on scene graph object
    SetTextureIdMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, GetImplementation(mImage).GetResourceId() );
  }
}

void ShaderEffect::Disconnect()
{
  DALI_ASSERT_DEBUG(mConnectionCount > 0);
  --mConnectionCount;

  if (mImage && mConnectionCount == 0)
  {
     GetImplementation(mImage).Disconnect();
  }
}

bool ShaderEffect::IsSceneObjectRemovable() const
{
  return false; // The Shader is not removed during this proxy's lifetime
}

unsigned int ShaderEffect::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void ShaderEffect::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const string& ShaderEffect::GetDefaultPropertyName(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_NAMES[index];
  }
  else
  {
    // index out of range..return empty string
    static const string INVALID_PROPERTY_NAME;
    return INVALID_PROPERTY_NAME;
  }
}

Property::Index ShaderEffect::GetDefaultPropertyIndex(const string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Lazy initialization of static mDefaultPropertyLookup
  if (!mDefaultPropertyLookup)
  {
    mDefaultPropertyLookup = new DefaultPropertyLookup();

    for (int i=0; i<DEFAULT_PROPERTY_COUNT; ++i)
    {
      (*mDefaultPropertyLookup)[DEFAULT_PROPERTY_NAMES[i]] = i;
    }
  }
  DALI_ASSERT_DEBUG( NULL != mDefaultPropertyLookup );

  // Look for name in default properties
  DefaultPropertyLookup::const_iterator result = mDefaultPropertyLookup->find( name );
  if ( mDefaultPropertyLookup->end() != result )
  {
    index = result->second;
  }

  return index;
}

bool ShaderEffect::IsDefaultPropertyWritable(Property::Index index) const
{
  return true;
}

bool ShaderEffect::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return false;
}

bool ShaderEffect::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return false;
}

Property::Type ShaderEffect::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_TYPES[index];
  }
  else
  {
    // index out of range...return Property::NONE
    return Property::NONE;
  }
}

void ShaderEffect::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  switch ( index )
  {
    case Dali::ShaderEffect::GRID_DENSITY:
    {
      SetGridDensityMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, propertyValue.Get<float>() );
      break;
    }

    case Dali::ShaderEffect::IMAGE:
    {
      Dali::Image img(Scripting::NewImage( propertyValue ));
      if(img)
      {
        SetEffectImage( img );
      }
      else
      {
        DALI_LOG_WARNING("Cannot create image from property value for ShaderEffect image\n");
      }
      break;
    }

    case Dali::ShaderEffect::PROGRAM:
    {
      string vertexPrefix   = GetShader("vertex-prefix", propertyValue);
      string fragmentPrefix = GetShader("fragment-prefix", propertyValue);
      string vertex         = GetShader("vertex", propertyValue);
      string fragment       = GetShader("fragment", propertyValue);

      GeometryType geometryType      = GEOMETRY_TYPE_IMAGE;

      if( propertyValue.HasKey("geometry-type") )
      {
        Property::Value geometryValue  = propertyValue.GetValue("geometry-type");
        DALI_ASSERT_ALWAYS(geometryValue.GetType() == Property::STRING && "Geometry type is not a string" );

        string s = geometryValue.Get<string>();
        if(s == "GEOMETRY_TYPE_IMAGE")
        {
          geometryType  = GEOMETRY_TYPE_IMAGE;
        }
        else if (s == "GEOMETRY_TYPE_TEXT")
        {
          geometryType  = GEOMETRY_TYPE_TEXT;
        }
        else if( s == "GEOMETRY_TYPE_MESH")
        {
          geometryType  = GEOMETRY_TYPE_MESH;
        }
        else if( s == "GEOMETRY_TYPE_TEXTURED_MESH")
        {
          geometryType  = GEOMETRY_TYPE_TEXTURED_MESH;
        }
        else
        {
          DALI_ASSERT_ALWAYS(!"Geometry type unknown" );
        }
      }
      SetPrograms( geometryType, vertexPrefix, vertex, fragmentPrefix, fragment );
      break;
    }

    case Dali::ShaderEffect::GEOMETRY_HINTS:
    {
      Dali::ShaderEffect::GeometryHints hint = Dali::ShaderEffect::HINT_NONE;
      Property::Value geometryHintsValue   = propertyValue.GetValue("geometry-hints");

      string s = geometryHintsValue.Get<string>();
      if(s == "HINT_NONE")
      {
        hint = Dali::ShaderEffect::HINT_NONE;
      }
      else if(s == "HINT_GRID_X")
      {
        hint = Dali::ShaderEffect::HINT_GRID_X;
      }
      else if(s == "HINT_GRID_Y")
      {
        hint = Dali::ShaderEffect::HINT_GRID_Y;
      }
      else if(s == "HINT_GRID")
      {
        hint = Dali::ShaderEffect::HINT_GRID;
      }
      else if(s == "HINT_DEPTH_BUFFER")
      {
        hint = Dali::ShaderEffect::HINT_DEPTH_BUFFER;
      }
      else if(s == "HINT_BLENDING")
      {
        hint = Dali::ShaderEffect::HINT_BLENDING;
      }
      else if(s == "HINT_FIXED_VERTICES")
      {
        hint = Dali::ShaderEffect::HINT_FIXED_VERTICES;
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Geometry hint unknown" );
      }

      SetHintsMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, hint );

      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "ShaderEffect property enumeration out of range"); // should not come here
      break;
    }
  }
}

Property::Value ShaderEffect::GetDefaultProperty(Property::Index /*index*/) const
{
  // none of our properties are readable so return empty
  return Property::Value();
}

void ShaderEffect::InstallSceneObjectProperty( PropertyBase& newProperty, const string& name, unsigned int index )
{
  // Warning - the property is added to the Shader object in the Update thread and the meta-data is added in the Render thread (through a secondary message)

  // mSceneObject is being used in a separate thread; queue a message to add the property
  InstallCustomPropertyMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, newProperty ); // Message takes ownership

  // mSceneObject requires metadata for each custom property (uniform)
  UniformMeta* meta = UniformMeta::New( name, newProperty, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
  // mSceneObject is being used in a separate thread; queue a message to add the property
  InstallUniformMetaMessage( mUpdateManager.GetEventToUpdate(), *mSceneObject, *meta ); // Message takes ownership

  // Add entry to the metadata lookup
  mCustomMetadata[index] = meta;
}

const SceneGraph::PropertyOwner* ShaderEffect::GetSceneObject() const
{
  return mSceneObject;
}

const PropertyBase* ShaderEffect::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

  DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "Property index is invalid" );

  DALI_ASSERT_ALWAYS( entry->second.IsAnimatable() && "shader effect has only animatable properties" );

  return dynamic_cast<const PropertyBase*>( entry->second.GetSceneGraphProperty() );
}

const PropertyInputImpl* ShaderEffect::GetSceneObjectInputProperty( Property::Index index ) const
{
  CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

  DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "Property index is invalid" );

  DALI_ASSERT_ALWAYS( entry->second.IsAnimatable() && "shader effect has only animatable properties" );

  return entry->second.GetSceneGraphProperty();
}

void ShaderEffect::SetPrograms( GeometryType  geometryTypes,
                                const string& vertexShaderPrefix,
                                const string& vertexShader,
                                const string& fragmentShaderPrefix,
                                const string& fragmentShader )
{
  string emptyStr;

  if( geometryTypes & GEOMETRY_TYPE_IMAGE )
  {
    SetWrappedProgram( GEOMETRY_TYPE_IMAGE, SHADER_SUBTYPE_ALL, vertexShaderPrefix, fragmentShaderPrefix, vertexShader, fragmentShader );
  }
  else
  {
    SetWrappedProgram( GEOMETRY_TYPE_IMAGE, SHADER_SUBTYPE_ALL, emptyStr, emptyStr, emptyStr, emptyStr );
  }

  if( geometryTypes & GEOMETRY_TYPE_TEXT )
  {
    // Only change the default program, leaving the other sub-types as-is.
    SetWrappedProgram( GEOMETRY_TYPE_TEXT, SHADER_DEFAULT, vertexShaderPrefix, fragmentShaderPrefix, vertexShader, fragmentShader );
  }
  else
  {
    SetWrappedProgram( GEOMETRY_TYPE_TEXT, SHADER_DEFAULT, emptyStr, emptyStr, emptyStr, emptyStr );
  }

  if( geometryTypes & GEOMETRY_TYPE_TEXTURED_MESH )
  {
    SetWrappedProgram( GEOMETRY_TYPE_TEXTURED_MESH, SHADER_SUBTYPE_ALL, vertexShaderPrefix, fragmentShaderPrefix, vertexShader, fragmentShader );
  }
  else
  {
    SetWrappedProgram( GEOMETRY_TYPE_TEXTURED_MESH, SHADER_SUBTYPE_ALL, emptyStr, emptyStr, emptyStr, emptyStr );
  }

  if( geometryTypes & GEOMETRY_TYPE_MESH )
  {
    SetWrappedProgram( GEOMETRY_TYPE_MESH, SHADER_SUBTYPE_ALL, vertexShaderPrefix, fragmentShaderPrefix, vertexShader, fragmentShader );
  }
  else
  {
    SetWrappedProgram( GEOMETRY_TYPE_MESH, SHADER_SUBTYPE_ALL, emptyStr, emptyStr, emptyStr, emptyStr );
  }
}

void ShaderEffect::SetWrappedProgram( GeometryType geometryType, ShaderSubTypes subType,
                                      const string& vertexPrefix, const string& fragmentPrefix,
                                      const string& vertexSnippet, const string& fragmentSnippet )
{
  // create complete shader program strings for the given geometry type
  unsigned int index = 0;
  switch( geometryType )
  {
    case GEOMETRY_TYPE_IMAGE:
    {
      index = 0;
    }
    break;
    case GEOMETRY_TYPE_TEXT:
    {
      index = 1;
    }
    break;
    case GEOMETRY_TYPE_MESH:
    case GEOMETRY_TYPE_TEXTURED_MESH:
    {
      index = 2;
    }
    break;
    case GEOMETRY_TYPE_LAST:
    {
      DALI_ASSERT_DEBUG(0 && "Wrong geometry type");
    }
    break;
  }

  string vertexSource = vertexPrefix + customShaderWrappers[index].vertexShaderPrefix;
  string fragmentSource = fragmentPrefix + customShaderWrappers[index].fragmentShaderPrefix;

  // Append the custom vertex shader code if supplied, otherwise append the default
  if ( vertexSnippet.length() > 0 )
  {
    vertexSource.append( vertexSnippet );
  }
  else
  {
    vertexSource.append( customShaderWrappers[index].vertexShaderPostfix );
  }

  // Append the custom fragment shader code if supplied, otherwise append the default
  if ( fragmentSnippet.length() > 0 )
  {
    fragmentSource.append( fragmentSnippet );
  }
  else
  {
    fragmentSource.append( customShaderWrappers[index].fragmentShaderPostfix );
  }

  // Add the program
  SetProgramImpl( geometryType, subType, vertexSource, fragmentSource );
}

void ShaderEffect::SetProgramImpl( GeometryType geometryType, ShaderSubTypes subType,
                                   const string& vertexSource, const string& fragmentSource )
{
  ShaderEffect::FixedVertexShader fixedVertices = FLEXIBLE;

  if( (mGeometryHints & Dali::ShaderEffect::HINT_FIXED_VERTICES) != 0 )
  {
    fixedVertices = FIXED;
  }

  SetProgramImpl( geometryType, subType, vertexSource, fragmentSource, fixedVertices );
}

void ShaderEffect::SetProgramImpl( GeometryType geometryType, ShaderSubTypes subType,
                                   const string& vertexSource, const string& fragmentSource,
                                   ShaderEffect::FixedVertexShader fixedShader )
{
  // Load done asynchronously in update thread. SetProgram message below must be processed afterwards.
  // Therefore, resource manager cannot farm out the loading to the adaptor resource threads,
  // but must instead use synchronous loading via PlatformAbstraction::LoadFile()
  size_t shaderHash;
  ResourceTicketPtr ticket( mShaderFactory.Load(vertexSource, fragmentSource, shaderHash) );

  DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "ShaderEffect: SetProgram(geometryType %d subType:%d ticket.id:%d)\n", geometryType, subType, ticket->GetId() );

  bool areVerticesFixed = (fixedShader == ShaderEffect::FIXED);
  // Add shader program to scene-object using a message to the UpdateManager
  SetShaderProgramMessage( mUpdateManager, *mSceneObject, geometryType, subType, ticket->GetId(), shaderHash, areVerticesFixed );

  mTickets.push_back(ticket);       // add ticket to collection to keep it alive.
}


} // namespace Internal

} // namespace Dali
