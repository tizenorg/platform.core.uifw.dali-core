#ifndef __DALI_INTERNAL_RENDER_TASK_DEFAULTS_H__
#define __DALI_INTERNAL_RENDER_TASK_DEFAULTS_H__

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

namespace Dali
{

namespace Internal
{

class Actor;
class CameraActor;

/**
 * An interface for retrieving render-tasks defaults
 */
class RenderTaskDefaults
{
public:

  /**
   * Retrieve the default root actor.
   * @return The default root actor.
   */
  virtual Actor& GetDefaultRootActor() = 0;

  /**
   * Retrieve the default camera actor.
   * @return The default camera actor.
   */
  virtual CameraActor& GetDefaultCameraActor() = 0;

protected:

  /**
   * Virtual destructor
   */
  virtual ~RenderTaskDefaults(){}
};

} // namespace Internal

} // namespace Dali

#endif //__DALI_INTERNAL_RENDER_TASK_DEFAULTS_H__
