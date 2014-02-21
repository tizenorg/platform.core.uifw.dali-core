#ifndef __SCENE_GRAPH_DYNAMICS_CUBE_SHAPE_H__
#define __SCENE_GRAPH_DYNAMICS_CUBE_SHAPE_H__

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

// BASE CLASS HEADERS
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class DynamicsWorld;

class DynamicsCubeShape : public DynamicsShape
{
public:
  /**
   * Constructor.
   * @param[in] world The dynamics world object
   */
  DynamicsCubeShape(DynamicsWorld& world);

  /**
   * Destructor.
   */
  virtual ~DynamicsCubeShape();

  /**
   * Initialize the cube.
   * @param[in] world      The dynamics world object
   * @param[in] dimensions A Vector3 detailing the width, height and depth of the cube.
   */
  void Initialize(const Vector3& dimensions);

private:
  // unimplemented copy constructor and assignment operator
  DynamicsCubeShape(const DynamicsCubeShape&);
  DynamicsCubeShape& operator=(const DynamicsCubeShape&);

}; // class DynamicsCubeShape

// Messages for DynamicsCubeShape

inline void InitializeDynamicsCubeShapeMessage( EventToUpdate& eventToUpdate, const DynamicsCubeShape& shape, const Vector3& dimensions )
{
  typedef MessageValue1< DynamicsCubeShape, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shape, &DynamicsCubeShape::Initialize, dimensions );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_CUBE_SHAPE_H__
