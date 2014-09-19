#ifndef __DALI_INTERNAL_MULTI_POINT_EVENT_UTIL_H__
#define __DALI_INTERNAL_MULTI_POINT_EVENT_UTIL_H__

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
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

namespace Internal
{

namespace MultiPointEventUtil
{

#if defined(DEBUG_ENABLED)

extern bool HIERARCHY_GEOMETRY;
extern bool HIERARCHY_SENSITIVITY;
extern bool HIERARCHY_TOUCH_REQUIRED;
extern bool HIERARCHY_HOVER_REQUIRED;
extern bool HIERARCHY_HITTABLE;

/**
 * Prints out all the children of the given actor when debug is enabled.
 *
 * @param[in]  actor  The actor whose children to print.
 * @param[in]  level  The number of " | " to put in front of the children.
 */
void PrintChildren( Debug::Filter* logFilter, Dali::Actor actor, int level );

/**
 * Prints the entire hierarchy of the scene.
 */
void PrintHierarchy( Debug::Filter* logFilter );

#define PRINT_HIERARCHY(f) MultiPointEventUtil::PrintHierarchy(f)

#else // defined(DEBUG_ENABLED)

#define PRINT_HIERARCHY(f)

#endif // defined(DEBUG_ENABLED)

bool IsActuallySensitive( Actor* actor );

} // namespace MultiPointEventUtil

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MULTI_POINT_EVENT_UTIL_H__

