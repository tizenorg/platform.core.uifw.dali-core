#ifndef __DALI_RENDER_TASK_LIST_H__
#define __DALI_RENDER_TASK_LIST_H__

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

/**
 * @addtogroup CAPI_DALI_RENDER_TASKS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

class RenderTask;

namespace Internal DALI_INTERNAL
{
class RenderTaskList;
}

/**
 * @brief An ordered list of Dali::RenderTask%s.
 *
 * These tasks describe how the Dali scene should be rendered; @see Dali::RenderTask for more details.
 */
class RenderTaskList : public BaseHandle
{
public:

  /**
   * @brief Create an empty RenderTaskList handle.
   *
   * This can be initialised with Stage::GetRenderTaskList().
   */
  RenderTaskList();

  /**
   * @brief Downcast a handle to RenderTaskList handle.
   *
   * If handle points to a RenderTaskList the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle A handle to an object.
   * @return A handle to a RenderTaskList or an uninitialized handle.
   */
  static RenderTaskList DownCast( BaseHandle handle );

  /**
   * @brief Virtual destructor.
   */
  virtual ~RenderTaskList();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Create a new RenderTask.
   *
   * This will be appended to the list of render-tasks.
   * @return A valid handle to a new RenderTask.
   */
  RenderTask CreateTask();

  /**
   * @brief Remove a RenderTask from the list of render-tasks.
   * @param[in] task The render-task to remove.
   */
  void RemoveTask( RenderTask task );

  /**
   * @brief Query the number of render-tasks in the list.
   *
   * This is ordered i.e. the task with index 0 is the first to be processed each frame.
   * @return The number of render-tasks.
   */
  unsigned int GetTaskCount() const;

  /**
   * @brief Retrieve a render-task.
   *
   * @pre index should be in range i.e. less than GetTaskCount().
   * @param[in] index The index of the render task to retrieve
   * @return A handle to the render-task.
   */
  RenderTask GetTask( unsigned int index ) const;

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods
   * @param [in] renderTask A pointer to a newly allocated render-task
   */
  explicit DALI_INTERNAL RenderTaskList( Internal::RenderTaskList* renderTask );
};

} // namespace Dali

/**
 * @}
 */
#endif //__DALI_RENDER_TASK_LIST_H__
