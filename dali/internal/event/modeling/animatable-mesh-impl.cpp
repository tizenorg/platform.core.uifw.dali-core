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
#include <dali/internal/event/modeling/animatable-mesh-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/modeling/scene-graph-animatable-mesh.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

//              Name              Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "position",        VECTOR3,  true,    true,     true,    Dali::AnimatableVertex::Property::POSITION       )
DALI_PROPERTY( "vertex-color",    VECTOR4,  true,    true,     true,    Dali::AnimatableVertex::Property::COLOR          )
DALI_PROPERTY( "texture-coords",  VECTOR2,  true,    true,     true,    Dali::AnimatableVertex::Property::TEXTURE_COORDS )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX )

} // namespace

AnimatableMesh::AnimatableMesh(
  EventThreadServices& eventThreadServices,
  SceneGraph::AnimatableMesh* sceneObject,
  MeshIPtr mesh,
  int numberOfVertices )
: mEventThreadServices(eventThreadServices),
  mSceneObject( sceneObject ),
  mMesh( mesh ),
  mNumberOfVertices( numberOfVertices ),
  mPropertyCount( numberOfVertices * DEFAULT_PROPERTY_COUNT )
{
  // Transfer animatable ownership to a scene message
  AddAnimatableMeshMessage( mEventThreadServices.GetUpdateManager(), *mSceneObject );
}

AnimatableMeshPtr AnimatableMesh::New(
  unsigned int numVertices,
  const Dali::AnimatableMesh::Faces& faceIndices )
{
  return New( numVertices, faceIndices, Dali::Material::New("dummy material"), true );
}

AnimatableMeshPtr AnimatableMesh::New(
  unsigned int numVertices,
  const Dali::AnimatableMesh::Faces& faceIndices,
  Dali::Material material)
{
  return New( numVertices, faceIndices, material, false );
}

AnimatableMeshPtr AnimatableMesh::New(
  unsigned int numVertices,
  const Dali::AnimatableMesh::Faces& faceIndices,
  Dali::Material material,
  bool useVertexColor )
{
  DALI_ASSERT_ALWAYS( numVertices > 0 && "Mesh has no vertices" );
  DALI_ASSERT_ALWAYS( ( numVertices * 3 ) < DEFAULT_PROPERTY_MAX_COUNT && "Mesh exceeds maximum supported vertices" );
  DALI_ASSERT_ALWAYS( faceIndices.size() > 0 && "Mesh has no faces" );
  for( Dali::AnimatableMesh::FacesConstIter faceIter=faceIndices.begin();
       faceIter != faceIndices.end();
       ++faceIter )
  {
    unsigned int faceIndex = *faceIter;
    DALI_ASSERT_ALWAYS( faceIndex < numVertices && "Face index out of range" );
  }

  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ResourceManager& resourceManager = tls.GetResourceManager();

  Dali::MeshData meshData;

  // We need a valid mesh data object to create the scene graph mesh
  Dali::MeshData::VertexContainer vertices( numVertices );
  BoneContainer bones;
  meshData.SetData(vertices, faceIndices, bones, material);
  bool useColor = false;
  if(useVertexColor)
  {
    useColor=true;
  }
  meshData.SetHasColor(useColor);
  meshData.SetHasNormals(false);
  meshData.SetHasTextureCoords(true);

  MeshIPtr mesh( Mesh::New( meshData, false /* not discardable, data is updated in the update thread */, true /* Scaling is required */ ) );

  // Create the scene object
  SceneGraph::AnimatableMesh* sceneObject = new SceneGraph::AnimatableMesh( resourceManager, mesh->GetResourceId(), meshData.GetVertices() );

  Stage* stage = Stage::GetCurrent();

  // Create the event object
  AnimatableMeshPtr animatableMeshPtr( new AnimatableMesh( *stage, sceneObject, mesh, meshData.GetVertexCount() ) );

  return animatableMeshPtr;
}

AnimatableMesh::~AnimatableMesh()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    if( mSceneObject )
    {
      RemoveAnimatableMeshMessage( mEventThreadServices.GetUpdateManager(), *mSceneObject );
    }
  }
}

unsigned int AnimatableMesh::GetNumberOfVertices() const
{
  return mNumberOfVertices;
}

Property::Index AnimatableMesh::GetVertexPropertyIndex(
  unsigned int vertex,
  Property::Index property ) const
{
  DALI_ASSERT_DEBUG(property < DEFAULT_PROPERTY_COUNT );
  return ( vertex * DEFAULT_PROPERTY_COUNT ) + property;
}

void AnimatableMesh::SetPosition( unsigned int vertexIndex, const Vector3& position)
{
  BakeVertexPositionMessage( mEventThreadServices, *mSceneObject, vertexIndex, position );
}

void AnimatableMesh::SetColor( unsigned int vertexIndex, const Vector4& color)
{
  BakeVertexColorMessage( mEventThreadServices, *mSceneObject, vertexIndex, color );
}

void AnimatableMesh::SetTextureCoords( unsigned int vertexIndex, const Vector2& coords)
{
  BakeVertexTextureCoordsMessage( mEventThreadServices, *mSceneObject, vertexIndex, coords );
}

const Vector3& AnimatableMesh::GetCurrentPosition( unsigned int vertexIndex) const
{
  return mSceneObject->GetPosition(mEventThreadServices.GetEventBufferIndex(), vertexIndex);
}

const Vector4& AnimatableMesh::GetCurrentColor( unsigned int vertexIndex) const
{
  return mSceneObject->GetColor(mEventThreadServices.GetEventBufferIndex(), vertexIndex);
}

const Vector2& AnimatableMesh::GetCurrentTextureCoords( unsigned int vertexIndex) const
{
  return mSceneObject->GetTextureCoords(mEventThreadServices.GetEventBufferIndex(), vertexIndex);
}

MeshIPtr AnimatableMesh::GetMesh()
{
  return mMesh;
}

/********************************************************************************
 ********************************************************************************
 ********************************   PROPERTY METHODS   **************************
 ********************************************************************************
 ********************************************************************************/

bool AnimatableMesh::Supports( Capability capability ) const
{
  return false; // The scene object is permanently "on stage" whilst this object is alive
}

unsigned int AnimatableMesh::GetDefaultPropertyCount() const
{
  return mPropertyCount;
}

void AnimatableMesh::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( mPropertyCount );

  for ( int i = 0; i < mPropertyCount; ++i )
  {
    indices.push_back( i );
  }
}

const char* AnimatableMesh::GetDefaultPropertyName( Property::Index index ) const
{
  if ( ( index >= 0 ) && ( index < mPropertyCount ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index % DEFAULT_PROPERTY_COUNT].name;
  }
  else
  {
    return NULL;
  }
}

Property::Index AnimatableMesh::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
    {
      index = i;
      break;
    }
  }
  return index;
}

bool AnimatableMesh::IsDefaultPropertyWritable(Property::Index index) const
{
  return DEFAULT_PROPERTY_DETAILS[ index % DEFAULT_PROPERTY_COUNT ].writable;
}

bool AnimatableMesh::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return DEFAULT_PROPERTY_DETAILS[ index % DEFAULT_PROPERTY_COUNT ].animatable;
}

bool AnimatableMesh::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return DEFAULT_PROPERTY_DETAILS[ index % DEFAULT_PROPERTY_COUNT ].constraintInput;
}

Property::Type AnimatableMesh::GetDefaultPropertyType(Property::Index index) const
{
  if ( ( index >= 0 ) && ( index < mPropertyCount ) )
  {
    return DEFAULT_PROPERTY_DETAILS[ index % DEFAULT_PROPERTY_COUNT ].type;
  }

  // Index out-of-bounds
  return Property::NONE;
}

void AnimatableMesh::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  DALI_ASSERT_ALWAYS( ( index < mPropertyCount ) && "Property index out of bounds" );

  int vertexProperty = index % DEFAULT_PROPERTY_COUNT;
  int vertexIndex    = index / DEFAULT_PROPERTY_COUNT;
  switch ( vertexProperty )
  {
    case Dali::AnimatableVertex::Property::POSITION:
    {
      SetPosition( vertexIndex, property.Get<Vector3>() );
      break;
    }
    case Dali::AnimatableVertex::Property::COLOR:
    {
      SetColor( vertexIndex, property.Get<Vector4>() );
      break;
    }
    case Dali::AnimatableVertex::Property::TEXTURE_COORDS:
    {
      SetTextureCoords( vertexIndex, property.Get<Vector2>() );
      break;
    }
    default:
    {
      // nothing to do
      break;
    }
  }
}

Property::Value AnimatableMesh::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;
  int vertexProperty = index % DEFAULT_PROPERTY_COUNT;
  int vertexIndex    = index / DEFAULT_PROPERTY_COUNT;

  switch ( vertexProperty )
  {
    case Dali::AnimatableVertex::Property::POSITION:
    {
      value = GetCurrentPosition(vertexIndex);
      break;
    }
    case Dali::AnimatableVertex::Property::COLOR:
    {
      value = GetCurrentColor(vertexIndex);
      break;
    }
    case Dali::AnimatableVertex::Property::TEXTURE_COORDS:
    {
      value = GetCurrentTextureCoords(vertexIndex);
      break;
    }
    default:
    {
      // nothing to do
      break;
    }
  }

  return value;
}

const SceneGraph::PropertyOwner* AnimatableMesh::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* AnimatableMesh::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );

  const SceneGraph::PropertyBase* property( NULL );

  // This method should only return a property which is part of the scene-graph
  if( mSceneObject != NULL )
  {
    int vertexProperty = index % DEFAULT_PROPERTY_COUNT;
    int vertexIndex    = index / DEFAULT_PROPERTY_COUNT;

    switch ( vertexProperty )
    {
      case Dali::AnimatableVertex::Property::POSITION:
        property = &mSceneObject->mVertices[vertexIndex].position;
        break;
      case Dali::AnimatableVertex::Property::COLOR:
        property = &mSceneObject->mVertices[vertexIndex].color;
        break;
      case Dali::AnimatableVertex::Property::TEXTURE_COORDS:
        property = &mSceneObject->mVertices[vertexIndex].textureCoords;
        break;
    }
  }

  return property;
}

const PropertyInputImpl* AnimatableMesh::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property( NULL );
  if( mSceneObject != NULL )
  {
    int vertexProperty = index % DEFAULT_PROPERTY_COUNT;
    int vertexIndex    = index / DEFAULT_PROPERTY_COUNT;

    switch ( vertexProperty )
    {
      case Dali::AnimatableVertex::Property::POSITION:
        property = &mSceneObject->mVertices[vertexIndex].position;
        break;
      case Dali::AnimatableVertex::Property::COLOR:
        property = &mSceneObject->mVertices[vertexIndex].color;
        break;
      case Dali::AnimatableVertex::Property::TEXTURE_COORDS:
        property = &mSceneObject->mVertices[vertexIndex].textureCoords;
        break;
    }
  }

  return property;
}

} // internal
} // dali
