#ifndef __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__

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
#include <dali/internal/common/message-buffer.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Node values (position etc.) are double-buffered.  A SceneGraphBuffers object
 * can be used to keep track of which buffers are being written or read.
 */
class SceneGraphBuffers
{
public:

  static BufferIndex INITIAL_EVENT_BUFFER_INDEX;  // 10b
  static BufferIndex INITIAL_RENDER_BUFFER_INDEX; // 0

  enum Accessing
  {
    UPDATE,
    RENDER,
    MAX_ACCESSING
  };

  /**
   * @brief Record start of thread accessing MessageBuffers.
   *
   * @param[in] accessing The thread that is recording start of access.
   * @param[in] buffer The message buffer that is being accessed.
   */
  void StartedAccess( Accessing accessing, MessageBuffer* buffer );

  /**
   * @brief Record end of thread accessing MessageBuffers.
   *
   * @param[in] accessing The thread that is recording the end of access.
   */
  void EndedAccess( Accessing accessing );

  /**
   * @brief Check for simultaneous access to message buffers.
   */
  void SanityCheck();

  /**
   * Create a SceneGraphBuffers object.
   */
  SceneGraphBuffers();

  /**
   * Non-virtual destructor; not intended as a base class.
   */
  ~SceneGraphBuffers();

  /**
   * Retrieve the current event-buffer index.
   * @return The buffer index.
   */
  BufferIndex GetEventBufferIndex() const { return mEventBufferIndex & 1; }

  /**
   * Retrieve the current update-buffer index.
   * @return The buffer index.
   */
  BufferIndex GetUpdateBufferIndex() const { return mEventBufferIndex >> 1; }

  BufferIndex GetRenderBufferIndex() const { return mRenderBufferIndex; }

  /**
   * Swap the Event & Update buffer indices.
  */
  void Swap();

  void SwapRenderBuffers();

private:

  // Undefined
  SceneGraphBuffers(const SceneGraphBuffers&);

  // Undefined
  SceneGraphBuffers& operator=(const SceneGraphBuffers& rhs);

private:

  BufferIndex mEventBufferIndex;  ///< bit 0 event thread buffer, bit 1 update thread buffer
  BufferIndex mRenderBufferIndex; ///< 0 or 1 (flipped by render thread)

  MessageBuffer* mMessageBuffers[ MAX_ACCESSING ]; ///< Message buffer being used for a thread.
  int mBalance;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__
