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

#include <dali/internal/common/owner-container.h>
#include <dali/internal/update/common/discard-queue.h>

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
template< class TypePointer >
class ObjectOwnerContainer
{
public:
  typedef typename Internal::OwnerContainer< TypePointer > ObjectContainer;
  typedef typename Internal::OwnerContainer< TypePointer >::Iterator Iterator;

  ObjectOwnerContainer( SceneGraphBuffers& sceneGraphBuffers, DiscardQueue& discardQueue )
  : mSceneGraphBuffers( sceneGraphBuffers ),
    mDiscardQueue( discardQueue )
  {
  }

  void Add( TypePointer object )
  {
    mObjectContainer.PushBack( object );
  }

  void Remove( TypePointer object )
  {
    DALI_ASSERT_DEBUG( object != NULL);

    // Find the object
    for( Iterator iter = mObjectContainer.Begin(); iter != mObjectContainer.End(); ++iter )
    {
      TypePointer current = *iter;
      if ( current == object )
      {
        // Transfer ownership to the discard queue
        // This keeps the object alive, until the render-thread has finished with it
        mDiscardQueue.Add( mSceneGraphBuffers.GetUpdateBufferIndex(), mObjectContainer.Release( iter ) );

        return;
      }
    }

    // Should not reach here
    DALI_ASSERT_DEBUG(false);
  }

  void ResetToBaseValues( BufferIndex bufferIndex )
  {
    for ( Iterator iter = mObjectContainer.Begin(); iter != mObjectContainer.End(); ++iter)
    {
      TypePointer object = (*iter);
      object->ResetToBaseValues( bufferIndex );
    }
  }

  unsigned int ConstrainObjects( BufferIndex bufferIndex )
  {
    unsigned int numberOfConstrainedObjects = 0;

    for ( Iterator iter = mObjectContainer.Begin(); iter != mObjectContainer.End(); ++iter)
    {
      TypePointer object = (*iter);
      numberOfConstrainedObjects += ConstrainPropertyOwner( *object, bufferIndex );
    }
    return numberOfConstrainedObjects;
  }

private:
  ObjectContainer mObjectContainer;
  SceneGraphBuffers& mSceneGraphBuffers;
  DiscardQueue& mDiscardQueue;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali


#endif // DALI_INTERNAL_SCENE_GRAPH_OBJECT_OWNER_CONTAINER
