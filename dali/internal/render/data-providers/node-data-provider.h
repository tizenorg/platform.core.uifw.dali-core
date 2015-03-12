#ifndef __DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H__

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

namespace Dali
{
struct Vector4;
class Matrix;

namespace Internal
{
namespace SceneGraph
{

/**
 * An interface to provide data for a Renderer
 */
class NodeDataProvider
{
public:

  /**
   * Constructor. Nothing to do as a pure interface.
   */
  NodeDataProvider() { }

  /**
   * @param bufferId to use
   * @return a reference to the model matrix
   */
  virtual const Matrix& GetModelMatrix( unsigned int bufferId ) = 0;

  /**
   * @param bufferId to use
   * @return a reference to the color
   */
  virtual const Vector4& GetRenderColor( unsigned int bufferId ) = 0;

  /**
   * @param[in] bufferIndex The buffer index to use
   * @return the actor size
   */
  virtual const Vector3& GetRenderSize( unsigned int bufferIndex ) = 0;

protected:
  /**
   * Virtual destructor, this is an interface, no deletion through this interface
   */
  virtual ~NodeDataProvider() { }
};

} // SceneGraph
} // Internal
} // Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H__
