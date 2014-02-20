#ifndef __DALI_INTEGRATION_DYNAMICS_DEBUG_VERTEX_H__
#define __DALI_INTEGRATION_DYNAMICS_DEBUG_VERTEX_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL HEADERS
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{

namespace Integration
{

struct DynamicsDebugVertex
{
  Vector3 position;
  Vector4 color;
}; // struct DynamicsDebugVertex

typedef std::vector<DynamicsDebugVertex> DynamicsDebugVertexContainer;

} // namespace Integration

} // namespace Dali

#endif /* __DALI_INTEGRATION_DYNAMICS_DEBUG_VERTEX_H__ */
