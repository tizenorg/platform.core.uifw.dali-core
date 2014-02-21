#ifndef __DALI_INTERNAL_RENDER_ALGORITHMS_H__
#define __DALI_INTERNAL_RENDER_ALGORITHMS_H__

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
#include <dali/internal/common/buffer-index.h>

namespace Dali
{

namespace Internal
{

class Context;

namespace SceneGraph
{
class RenderInstruction;
}

namespace Render
{

/**
 * Process a render-instruction.
 * @param[in] instruction The render-instruction to process.
 * @param[in] context The GL context.
 * @param[in] buffer The current render buffer index (previous update buffer)
 * @param[in] frameTime The elapsed time between the last two updates.
 */
void ProcessRenderInstruction( const SceneGraph::RenderInstruction& instruction,
                               Context& context,
                               BufferIndex bufferIndex,
                               float frameTime );

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDER_ALGORITHMS_H__

