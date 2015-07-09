#ifndef __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_ALGORITHMS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_ALGORITHMS_H__

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
#include <dali/internal/update/manager/sorted-layers.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Clear the list of opaque/transparent renderables for each layer.
 * @param[in] sortedLayers The layers containing lists of opaque/transparent renderables.
 */
void ClearRenderables( SortedLayerPointers& sortedLayers );

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_ALGORITHMS_H__

