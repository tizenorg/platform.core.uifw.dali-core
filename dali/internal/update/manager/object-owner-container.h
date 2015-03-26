#ifndef DALI_INTERNAL_SCENE_GRAPH_OBJECT_OWNER_CONTAINER
#define DALI_INTERNAL_SCENE_GRAPH_OBJECT_OWNER_CONTAINER

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
 */

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/internal/common/owner-container.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/controllers/scene-controller.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class UpdateManager;

/**
 * ObjectOwnerContainer is an object which owns SceneGraph Objects.
 * It is responsible for ensuring they are placed on a discard queue
 * when removed from the container.
 */
template< class Type >
class ObjectOwnerContainer
{
public:
  typedef typename Internal::OwnerContainer< Type* > ObjectContainer;
  typedef typename Internal::OwnerContainer< Type* >::Iterator Iterator;

  ObjectOwnerContainer( SceneGraphBuffers& sceneGraphBuffers, DiscardQueue& discardQueue )
  : mSceneController( NULL ),
    mSceneGraphBuffers( sceneGraphBuffers ),
    mDiscardQueue( discardQueue )
  {
  }

  void SetSceneController(SceneController& sceneController)
  {
    mSceneController = &sceneController;
  }

  void Add( Type* pointer )
  {
    DALI_ASSERT_DEBUG( pointer && "Pointer should not be null" );

    mObjectContainer.PushBack( pointer );

    pointer->ConnectToSceneGraph(*mSceneController, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  void Remove( Type* pointer )
  {
    DALI_ASSERT_DEBUG( pointer && "Pointer should not be null" );

    // Find the object
    Iterator match = std::find( mObjectContainer.Begin(), mObjectContainer.End(), pointer );
    DALI_ASSERT_DEBUG( match != mObjectContainer.End() && "Should always find a match" );

    mDiscardQueue.Add( mSceneGraphBuffers.GetUpdateBufferIndex(), mObjectContainer.Release( match ) );
    pointer->DisconnectFromSceneGraph(*mSceneController, mSceneGraphBuffers.GetUpdateBufferIndex() );
  }

  void ResetToBaseValues( BufferIndex bufferIndex )
  {
    for ( Iterator iter = mObjectContainer.Begin(); iter != mObjectContainer.End(); ++iter)
    {
      Type* object = (*iter);
      object->ResetToBaseValues( bufferIndex );
    }
  }

  unsigned int ConstrainObjects( BufferIndex bufferIndex )
  {
    unsigned int numberOfConstrainedObjects = 0;

    for ( Iterator iter = mObjectContainer.Begin(); iter != mObjectContainer.End(); ++iter)
    {
      Type* object = (*iter);
      numberOfConstrainedObjects += ConstrainPropertyOwner( *object, bufferIndex );
    }
    return numberOfConstrainedObjects;
  }

private:
  SceneController* mSceneController;
  ObjectContainer mObjectContainer;
  SceneGraphBuffers& mSceneGraphBuffers;
  DiscardQueue& mDiscardQueue;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali


#endif // DALI_INTERNAL_SCENE_GRAPH_OBJECT_OWNER_CONTAINER
