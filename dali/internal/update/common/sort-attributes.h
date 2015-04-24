#ifndef DALI_INTERNAL_SCENE_GRAPH_COMMON_SORT_ATTRIBUTES_H
#define DALI_INTERNAL_SCENE_GRAPH_COMMON_SORT_ATTRIBUTES_H

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

#include <cstdio>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Shader;
class Material;
class Geometry;

struct SortAttributes
{
  SortAttributes()
  : depthIndex(0),
    zValue(0.0f),
    shader(NULL),
    material(NULL),
    geometry(NULL)
  {
  }

  SortAttributes(int depth, float z)
  : depthIndex(depth),
    zValue(z),
    shader(NULL),
    material(NULL),
    geometry(NULL)
  {
  }

  int depthIndex; // The depth index for the given renderer
  float zValue;   // The zValue of the given renderer (either distance from camera, or a custom calculated value)
  Shader* shader;
  Material* material;
  Geometry* geometry;
};

}//namespace SceneGraph
}//namespace Internal
}//namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_COMMON_SORT_ATTRIBUTES_H
