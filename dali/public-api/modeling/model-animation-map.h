#ifndef __DALI_MODEL_ANIMATION_MAP_H__
#define __DALI_MODEL_ANIMATION_MAP_H__

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
#include <dali/public-api/modeling/entity-animator-map.h>

namespace Dali DALI_IMPORT_API
{

struct ModelAnimationMap;
typedef std::vector<ModelAnimationMap> ModelAnimationMapContainer;

/**
 * @brief ModelAnimationMap holds a set of entity animators for a given named animation,
 * along with animation properties such as duration, repeats, etc.
 */
struct ModelAnimationMap
{
  /**
   * @brief Destructor.
   **/
  ModelAnimationMap()
    : duration(0.0f),
      repeats(0)
  {
  }

  std::string                   name;
  EntityAnimatorMapContainer    animators;
  float                         duration;
  int                           repeats;
};

} // namespace Dali

#endif // __DALI_MODEL_ANIMATION_MAP_H__
