#ifndef __DALI_INTERNAL_SCENEGRAPH_RENDERER_DEBUG_H
#define __DALI_INTERNAL_SCENEGRAPH_RENDERER_DEBUG_H

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

#include <dali/public-api/math/matrix.h>

namespace Dali
{
namespace Internal
{
class Context;

namespace SceneGraph
{
class RenderDataProvider;

void DebugBoundingBox( Context& context, RenderDataProvider& dataProvider, const Matrix& model, const Matrix& viewMatrix, const Matrix& projectionMatrix);

#if defined(DEBUG_ENABLED)
#define DEBUG_BOUNDING_BOX( context, dataProvider, modelMatrix, viewMatrix, projectionMatrix ) \
  DebugBoundingBox( context, dataProvider, modelMatrix, viewMatrix, projectionMatrix)
#else
#define DEBUG_BOUNDING_BOX( context, dataProvider, modelMatrix, viewMatrix, projectionMatrix )
#endif

} // SceneGraph
} // Internal
} // Dali


#endif // __DALI_INTERNAL_SCENEGRAPH_RENDERER_DEBUG_H
