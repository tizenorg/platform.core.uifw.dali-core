#ifndef __DALI_INTERNAL_RESOURCE_TYPE_PATH_ID_MAP_H__
#define __DALI_INTERNAL_RESOURCE_TYPE_PATH_ID_MAP_H__

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
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/internal/event/resources/resource-type-path.h>

namespace Dali
{

namespace Internal
{

///< A map of resource IDs sorted by ResourceTypePath
typedef std::map<ResourceTypePath, unsigned int>  ResourceTypePathIdMap;
typedef ResourceTypePathIdMap::iterator           ResourceTypePathIdIter;
typedef ResourceTypePathIdMap::size_type          ResourceTypePathIdSize;
typedef std::pair<ResourceTypePath, unsigned int> ResourceTypePathIdPair;

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_TYPE_PATH_ID_MAP_H__
