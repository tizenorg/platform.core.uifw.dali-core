#ifndef __DALI_INTERNAL_COMPLETE_STATUS_MANAGER_H__
#define __DALI_INTERNAL_COMPLETE_STATUS_MANAGER_H__

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

#include <dali/public-api/common/dali-vector.h>
#include <dali/integration-api/resource-declarations.h>

namespace Dali
{

namespace Integration
{
class GlSyncAbstraction;
}

namespace Internal
{
class ResourceManager;
class ResourceTracker;

namespace SceneGraph
{
class RenderMessageDispatcher;
}

/**
 * Class to manage resource load completion status.
 *
 * This class uses ResourceManager to determine the complete status of loaded resources.
 */
class CompleteStatusManager
{
public:
  /**
   * Resource readiness state
   */
  enum CompleteState
  {
    NOT_READY, ///< Resource is not ready yet
    COMPLETE,  ///< Resource has finished loading, or is otherwise complete
    FAILED     ///< Resource load has failed.
  };

  /**
   * Constructor.
   * @param[in] resourceManager The resource manager (For handling untracked resources)
   */
  CompleteStatusManager( ResourceManager& resourceManager );

  /**
   * Destructor
   */
  ~CompleteStatusManager();

  /**
   * @param[in] id The resource id to track
   * @return true if the FBO has been rendered to, i.e. it can be considered "complete"
   */
  bool HasFrameBufferBeenRenderedTo( Integration::ResourceId id ) const;

  /**
   * @param[in] id of the framebuffer that has been rendered to
   */
  void SetFrameBufferBeenRenderedTo( Integration::ResourceId id );

  /**
   * Gets the complete status of the resource.  If it has a tracker,
   * it returns the status from the resource tracker, otherwise it
   * returns the load status from the resource manager
   *
   * @param[in] id The resource id @return The complete state of the resource
   * @return the CompleteState
   */
  CompleteState GetStatus( Integration::ResourceId id ) const;

private:

  ResourceManager& mResourceManager;   ///< The resource manager
  Dali::Vector< Integration::ResourceId > mFboRenderedIds; // the id's of FBOs that have been rendered to

};

} // Internal

} // Dali

#endif // __DALI_INTERNAL_COMPLETE_STATUS_MANAGER_H__
