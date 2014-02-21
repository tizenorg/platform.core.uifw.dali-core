#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_TRACKER_H
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_TRACKER_H

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

#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/internal/common/message.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * This class is used to communicate with Fence Sync objects, keeping access solely in
 * the RenderThread to avoid having mutual exclusion locks or messages back to the UpdateThread.
 */
class RenderTracker
{
public:
  /**
   * Constructor
   * @param[in] glSyncAbstraction The GlSyncAbstraction
   * @param[in] resourceId The resource ID of the framebuffer texture being tracked
   */
  RenderTracker(Integration::GlSyncAbstraction& glSyncAbstraction);

  /**
   * Destructor
   * Delete any outstanding sync objects?
   */
  ~RenderTracker();

  /**
   * Creates a sync object for this tracker. Will delete any existing sync object.
   */
  void CreateSyncObject();

  /**
   * Check the GL Sync objects. This is called from Render Thread.
   * If the GlSyncObject has been triggered, then atomically set the sync trigger
   */
  void PollSyncObject();

  /**
   * Check the sync trigger. This is called from Update Thread, so atomically reads the sync trigger
   * It clears the sync trigger if it was set.
   * @return true if the tracker has been synced
   */
  bool IsSynced();

  /**
   * Atomically reset the sync trigger. This may be called from any thread
   */
  void ResetSyncFlag();

  /**
   * Atomically sets the sync trigger. This may be called from any thread
   */
  void SetSyncFlag();

private:
  Integration::GlSyncAbstraction& mGlSyncAbstraction; // The sync abstraction
  volatile int mSyncTrigger;             // Trigger that update thread can read & reset
  Integration::GlSyncAbstraction::SyncObject* mSyncObject; // Associated sync object
};

} // SceneGraph
} // Internal
} // Dali


#endif // __DALI_INTERNAL_RENDER_TRACKER_H
