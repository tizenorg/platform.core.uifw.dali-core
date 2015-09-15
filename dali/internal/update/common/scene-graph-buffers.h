#ifndef __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__

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

  static BufferIndex INITIAL_EVENT_BUFFER_INDEX;  // 0
  static BufferIndex INITIAL_UPDATE_BUFFER_INDEX; // 1


  enum Accessing
  {
    UPDATE,
    RENDER,
    MAX_ACCESSING
  };

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
  BufferIndex GetEventBufferIndex() const { return mEventBufferIndex; }

  /**
   * Retrieve the current update-buffer index.
   * @return The buffer index.
   */
  BufferIndex GetUpdateBufferIndex() const { return mUpdateBufferIndex; }

  BufferIndex GetRenderBufferIndex() const { return mRenderBufferIndex; }

  /**
   * Swap the Event & Update buffer indices.
  */
  void Swap();

  void SwapRenderBuffers();

  /**
   * @brief Record start of thread accessing MessageBuffers.
   *
   * @param[in] accessing The thread that is recording start of access.
   * @param[in] buffer The message buffer that is being accessed.
   * @param[in] microseconds The current time stamp in microseconds.
   */
  void StartedAccess( Accessing accessing, MessageBuffer* buffer );

  /**
   * @brief Record end of thread accessing MessageBuffers.
   *
   * @param[in] accessing The thread that is recording the end of access.
   * @param[in] microseconds The current time stamp in microseconds.
   */
  void EndedAccess( Accessing accessing );

  /**
   * @brief Check for simultaneous access to message buffers.
   */
  void SanityCheck();

private:

  // Undefined
  SceneGraphBuffers(const SceneGraphBuffers&);

  // Undefined
  SceneGraphBuffers& operator=(const SceneGraphBuffers& rhs);

private:

  BufferIndex mEventBufferIndex;                   ///< 0 or 1 (opposite of mUpdateBufferIndex)
  BufferIndex mUpdateBufferIndex;                  ///< 0 or 1 (opposite of mEventBufferIndex)
  BufferIndex mRenderBufferIndex;
  MessageBuffer* mMessageBuffers[ MAX_ACCESSING ]; ///< Message buffer being used for a thread.
  int mBalance;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__
