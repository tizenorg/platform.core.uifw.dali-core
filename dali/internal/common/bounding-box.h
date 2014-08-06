#ifndef DALI_INTERNAL_BOUNDING_BOX_H
#define DALI_INTERNAL_BOUNDING_BOX_H

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
 */

#include <dali/public-api/math/vector3.h>

namespace Dali
{
namespace Internal
{

struct BoundingBox
{
  BoundingBox()
  {
    minVertex = Vector3::ZERO;
    maxVertex = Vector3::ZERO;
  }

  BoundingBox(const BoundingBox& boundingBox)
  {
    minVertex = boundingBox.minVertex;
    maxVertex = boundingBox.maxVertex;
  }

  BoundingBox& operator=( const BoundingBox& rhs )
  {
    minVertex = rhs.minVertex;
    maxVertex = rhs.maxVertex;
    return *this;
  }

  void SetBoundingBox( const BoundingBox& rhs )
  {
    minVertex = rhs.minVertex;
    maxVertex = rhs.maxVertex;
  }

  Vector3 minVertex;
  Vector3 maxVertex;
};

} // Internal
} // Dali

#endif // DALI_INTERNAL_BOUNDING_BOX_H
