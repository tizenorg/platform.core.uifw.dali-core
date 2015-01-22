#ifndef __DALI_INTERNAL_RENDER_MESH_OWNER_H__
#define __DALI_INTERNAL_RENDER_MESH_OWNER_H__

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

class RenderMesh;

/**
 * RenderMeshOwner is an interface to be implemented in a render thread class
 * that should deal with the ownership of RenderMesh objects.
 */
class RenderMeshOwner
{
public:
  /**
   * @brief Destructor
   */
  virtual ~RenderMeshOwner() {};

  /**
   * @brief Dispatch a message to the RenderMeshOwner to add a mesh
   *
   * @param[in] renderMesh to add
   */
  virtual void AddRenderMesh( RenderMesh* renderMesh ) = 0;

  /**
   * @brief Dispatch a message to the RenderMeshOwner to add a mesh
   *
   * @param[in] renderMesh to remove
   */
  virtual void RemoveRenderMesh( RenderMesh* renderMesh ) = 0;
};

} // SceneGraph

} // Internal

} // Dali

#endif // __DALI_INTERNAL_RENDER_MESH_OWNER_H__
