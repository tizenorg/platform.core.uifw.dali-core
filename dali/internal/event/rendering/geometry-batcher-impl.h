#ifndef __DALI_INTERNAL_GEOMETRY_BATCHER_H__
#define __DALI_INTERNAL_GEOMETRY_BATCHER_H__

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

#include <dali/public-api/actors/actor.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/devel-api/rendering/geometry-batcher.h>

namespace Dali
{
class GeometryBatcher;

namespace Internal
{

class GeometryBatcher : public BaseObject
{
public:

  static GeometryBatcher* New( const Dali::Actor& batchParent );

  GeometryBatcher( const Dali::Actor& batchParent );

  void UpdateGeometry( const Dali::Actor& actor, Dali::Geometry& geometry );

private:

  Dali::Actor mBatchParentActor;

  typedef std::pair<Dali::Actor, Dali::Geometry>      ActorGeometryPair;
  typedef std::vector<ActorGeometryPair>              GeometryArray;

  // set of geometries and actors
  GeometryArray mGeometries;

};

}

/**
 * Helper methods for public API.
 */
inline Internal::GeometryBatcher& GetImplementation(Dali::GeometryBatcher& batcher)
{
  DALI_ASSERT_ALWAYS( batcher && "GeometryBatcher handle is empty" );

  BaseObject& handle = batcher.GetBaseObject();

  return static_cast<Internal::GeometryBatcher&>(handle);
}

inline const Internal::GeometryBatcher& GetImplementation(const Dali::GeometryBatcher& batcher)
{
  DALI_ASSERT_ALWAYS( batcher && "GeometryBatcher handle is empty" );

  const BaseObject& handle = batcher.GetBaseObject();

  return static_cast<const Internal::GeometryBatcher&>(handle);
}

}

#endif
