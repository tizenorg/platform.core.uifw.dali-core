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

// CLASS HEADER
#include <dali/devel-api/rendering/geometry-batcher.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/internal/event/rendering/geometry-batcher-impl.h>

namespace Dali
{

GeometryBatcher::GeometryBatcher()
{

}

GeometryBatcher::~GeometryBatcher()
{

}

GeometryBatcher GeometryBatcher::New( const Actor &batchParentActor )
{
  return GeometryBatcher( Internal::GeometryBatcher::New( batchParentActor ) );
}

GeometryBatcher::GeometryBatcher( Internal::GeometryBatcher* internal ) :
  BaseHandle( internal )
{

}

void GeometryBatcher::UpdateGeometry( const Actor& actor, Geometry& geometry )
{
  GetImplementation(*this).UpdateGeometry( actor, geometry );
}

}
