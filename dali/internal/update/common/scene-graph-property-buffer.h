#ifndef DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H
#define DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H

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

#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/property-owner.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class SceneController;

class PropertyBuffer : public PropertyOwner
{
public:
  //@todo MESH_REWORK Remove when we have working property buffers
  static PropertyBuffer* NewQuadVertices();

  //@todo MESH_REWORK Remove when we have working property buffers
  static PropertyBuffer* NewQuadIndices();

  /**
   * Constructor
   */
  PropertyBuffer();

  /**
   * Destructor
   */
  virtual ~PropertyBuffer();

  /**
   * Get the size of the property buffer in bytes
   * @return the size in bytes
   */
  std::size_t GetDataSize( BufferIndex bufferIndex ) const;

  /**
   * Get the size of an element of the buffer in bytes
   * @return the element size in bytes
   */
  std::size_t GetElementSize( BufferIndex bufferIndex ) const;

  /**
   * Get the property buffer data
   * @return the property buffer's data array
   */
  const void* GetData( BufferIndex bufferIndex ) const;

  /**
   * Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * Disconnect the object from the scene graph
   */
  void DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

private:

  // @todo MESH_REWORK - TEMPORARY TYPES - REMOVE WHEN WE HAVE WORKING BUFFERS
  typedef Dali::Vector< char > CharBuffer;
  CharBuffer  mData;
  std::size_t mElementSize;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H
