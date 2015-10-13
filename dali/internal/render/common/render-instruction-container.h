#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_CONTAINER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_CONTAINER_H__

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
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/buffer-index.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderInstruction;

enum RenderInstructionDebugEvent
{
  RENDER_INSTRUCTION_READ_START,
  RENDER_INSTRUCTION_READ_END,
  RENDER_INSTRUCTION_WRITE_START,
  RENDER_INSTRUCTION_WRITE_END
};

enum RenderInstructionDebugState
{
  RENDER_INSTRUCTION_UNUSED,
  RENDER_INSTRUCTION_READING,
  RENDER_INSTRUCTION_WRITING
};

/**
 * Class to encapsulate double buffered render instruction data
 */
class RenderInstructionContainer
{
public:

  /**
   * Constructor
   */
  RenderInstructionContainer();

  /**
   * Destructor
   */
  ~RenderInstructionContainer();

  void DebugGuard( BufferIndex bufferIndex, RenderInstructionDebugEvent state )
  {
    if( RENDER_INSTRUCTION_WRITE_START == state )
    {
      DALI_ASSERT_ALWAYS( RENDER_INSTRUCTION_UNUSED == mDebugState[ bufferIndex ] && "Invalid write to RenderInstruction" );
      mDebugState[ state ] = RENDER_INSTRUCTION_WRITING;
    }
    else if( RENDER_INSTRUCTION_READ_START == state )
    {
      DALI_ASSERT_ALWAYS( RENDER_INSTRUCTION_UNUSED == mDebugState[ bufferIndex ] && "Invalid read to RenderInstruction" );
      mDebugState[ state ] = RENDER_INSTRUCTION_READING;
    }
    else
    {
      mDebugState[ state ] = RENDER_INSTRUCTION_UNUSED;
    }
  }

  /**
   * Reset the container index and reserve space in the container if needed
   * @param bufferIndex to reset
   * @param capacityRequired in the container
   */
  void ResetAndReserve( BufferIndex bufferIndex, size_t capacityRequired );

  /**
   * Return the count of instructions in the container
   * @param bufferIndex to use
   * @return the count of elements
   */
  size_t Count( BufferIndex bufferIndex );

  /**
   * Get a reference to the next instruction
   * @param bufferIndex to use
   */
  RenderInstruction& GetNextInstruction( BufferIndex bufferIndex );

  /**
   * Get a reference to the instruction at index
   * @param bufferIndex to use
   * @param index to use
   */
  RenderInstruction& At( BufferIndex bufferIndex, size_t index );

private:

  unsigned int mIndex[ 2 ]; ///< count of the elements that have been added
  typedef OwnerContainer< RenderInstruction* > InstructionContainer;
  InstructionContainer mInstructions[ 2 ]; /// Double buffered instruction lists
  RenderInstructionDebugState mDebugState[ 2 ]; ///< Sanity check that buffers read & write does not occur at the same time

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_CONTAINER_H__
