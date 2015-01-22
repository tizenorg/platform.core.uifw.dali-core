#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_QUEUE_DISPATCHER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_QUEUE_DISPATCHER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/render/queue/render-queue.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

// No parameters
template < typename Object >
void DispatchMessage( RenderQueue& renderQueue,
                      BufferIndex bufferIndex,
                      Object* object,
                      void(Object::*function)() )
{
  // Send a message to RenderMesh in render thread
  typedef Message< Object > MessageType;
  unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( MessageType ) );
  new (slot) MessageType( object, function );
}
/*
// One parameter
template < typename Object, typename P1 >
void DispatchMessage( RenderQueue& renderQueue,
                      BufferIndex bufferIndex,
                      Object* object,
                      void(Object::*function)(
                          typename ParameterType< P1 >::PassingType ),
                      typename ParameterType< P1 >::PassingType p1 )
{
  // Send a message to RenderMesh in render thread
  typedef MessageValue1< Object, P1 > MessageType;
  unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( MessageType ) );
  new (slot) MessageType( object, function, p1 );
}
*/
} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_QUEUE_DISPATCHER_H__
