#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROCESS_RENDER_TASKS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROCESS_RENDER_TASKS_H__

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

// INTERNAL INCLUDES
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/manager/prepare-render-instructions.h>

namespace Dali
{
namespace Internal
{
class CompleteStatusManager;

namespace SceneGraph
{

class RenderTaskList;

/**
 * Process the list of render-tasks; the output is a series of render instructions.
 * @note When ProcessRenderTasks is called, the layers should already the transparent/opaque attachments which are ready to render.
 * If there is only one default render-task, then no further processing is required.
 * @param[in] updateBufferIndex The current update buffer index.
 * @param[in] completeStatusManager The resource complete state manager
 * @param[in] renderTasks The list of render-tasks.
 * @param[in] rootNode The root node of the scene-graph.
 * @param[in] sortedLayers The layers containing lists of opaque/transparent renderables.
 * @param[in] sortingHelper Helper container for sorting transparent renderables.
 * @param[out] instructions The instructions for rendering the next frame.
 */
void ProcessRenderTasks( BufferIndex updateBufferIndex,
                         CompleteStatusManager& completeStatusManager,
                         RenderTaskList& renderTasks,
                         Layer& rootNode,
                         SortedLayerPointers& sortedLayers,
                         RendererSortingHelper& sortingHelper,
                         RenderInstructionContainer& instructions );

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROCESS_RENDER_TASKS_H__
