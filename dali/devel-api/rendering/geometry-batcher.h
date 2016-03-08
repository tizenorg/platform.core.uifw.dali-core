#ifndef __DALI_GEOMETRY_BATCHER_H__
#define __DALI_GEOMETRY_BATCHER_H__

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

// EXTERNAL INCLUDES
//#include <dali/internal/event/rendering/geometry-batcher-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/material.h>

#include <vector>
#include <utility>

namespace Dali
{

class Material;
class Actor;

namespace Internal
{
class GeometryBatcher;
}

class Material;
class Geometry;

// --------------------------------------------------------------------------
class GeometryBatcher : public BaseHandle
{

public:

  GeometryBatcher();

  /**
   * @brief New
   * creates new batcher instance with specified actor as parent
   * @param batchParentActor
   * @return
   */
  static GeometryBatcher New( const Actor& batchParentActor );

  ~GeometryBatcher();

  void UpdateGeometry( const Actor& actor, Geometry& geometry );

public:

  explicit DALI_INTERNAL GeometryBatcher( Internal::GeometryBatcher* );

};


}

#endif
