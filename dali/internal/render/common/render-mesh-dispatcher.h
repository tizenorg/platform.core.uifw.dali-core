#ifndef __DALI_INTERNAL_RENDER_MESH_OWNER_DISPATCHER_H__
#define __DALI_INTERNAL_RENDER_MESH_OWNER_DISPATCHER_H__

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
#include <dali/internal/common/buffer-index.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class RenderMeshOwner;
class RenderMesh;
class RenderQueue;

class RenderMeshDispatcher
{
public:
  /**
   * @brief Creates a RenderMeshOwnerDispatcher
   *
   * @param[in] renderQueue The queue to which to send messages
   * @param[in] renderMeshOwner RenderMeshOwner that will received the sent messages
   */
  RenderMeshDispatcher( RenderQueue& renderQueue,
                        RenderMeshOwner& renderMeshOwner );

  /**
   * @brief Dispatch a message to the RenderMeshOwner to add a mesh
   */
  void AddRenderMesh( BufferIndex updateBufferIndex, RenderMesh* renderMesh );

  /**
   * @brief Dispatch a message to the RenderMeshOwner to add a mesh
   */
  void RemoveRenderMesh( BufferIndex updateBufferIndex, RenderMesh* renderMesh );

private: // disabled methods
  RenderMeshDispatcher(const RenderMeshDispatcher&);
  RenderMeshDispatcher& operator=(const RenderMeshDispatcher&);

private:

  RenderQueue&      mRenderQueue;
  RenderMeshOwner&  mRenderMeshOwner;
};

} // SceneGraph

} // Internal

} // Dali

#endif // __DALI_INTERNAL_RENDER_MESH_OWNER_DISPATCHER_H__
